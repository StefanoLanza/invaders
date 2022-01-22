#include <random>
#include <chrono>
#include <ctime>
#include <cstring>
#include <thread>
#include <cassert>
#include <iostream>
#include <algorithm>

#include <engine/Base.h>
#include <engine/Vector2D.h>
#include <engine/Console.h>
#include <engine/Keyboard.h>
#include <engine/Input.h>
#include <engine/MessageLog.h>
#include <engine/Console.h>
#include <engine/DLL.h>
#include <inih-master/ini.h>

#include <game/Images.h>
#include <game/GameConfig.h>
#include <game/Game.h>
#include <game/PlayField.h>
#include <game/GameStates.h>
#include <game/GameStateMgr.h>
#include <game/StartMenu.h>
#include <game/PauseScreen.h>
#include <game/PlayGameState.h>
#include <game/GameOverState.h>
#include <game/IntroScreen.h>
#include <game/VictoryScreen.h>
#include <game/AIModule.h>

namespace
{
	
bool ReadGameConfig(GameConfig& config, const char* iniFile);
void RegisterGameStates(Game& game);

}


int main()
{
	std::cout << "IKA Invaders" << std::endl;
	std::cout << "by Stefano Lanza - steflanz@gmail.com - 2022" << std::endl;
	std::cout << std::endl;

	std::cout << "Reading game config from game.ini" << std::endl;
	GameConfig gameConfig;
	if (! ReadGameConfig(gameConfig, "game.ini")) {
		std::cerr << "Cannot read game config" << std::endl;
	}

	AIModule aiModule;
	const char* aiDLLFileName = 
	#ifdef WINDOWS
		"Scripts.dll";
	#else
		"./libScripts.so";
	#endif
	std::cout << "Initializing AI module" << std::endl;
	if (! InitAIModule(aiModule, aiDLLFileName))
	{
		std::cerr << "Cannot initialize DLL" << std::endl;
		return 1;
	}

	if (! InitKeyboard()) {
		std::cerr << "Failed to init keyboard" << std::endl;
		return 1;
	}

	std::cout << "Initializing console" << std::endl;
	Console console;
	if (! console.Initialize(gameConfig.consoleWidth, gameConfig.consoleHeight, gameConfig.fontSize))
	{
		std::cerr << "Cannot initialize console" << std::endl;
		return 1;
	}

	std::default_random_engine rGen;
	MessageLog messageLog;
	const Vector2D worldSize { (float)gameConfig.consoleWidth, (float)gameConfig.consoleHeight };
	PlayField world { worldSize, gameConfig, rGen };
	Game game = NewGame(world, gameConfig, rGen, messageLog, aiModule);
	RegisterGameStates(game);

	InitGameImages();

	EnterGameState(game, (int)GameStateId::start);

	// Simulation and rendering happen at a fixed rate
	const int fixedFrameTime = 16666; // [us]
	const float fixedDeltaTime = (float)fixedFrameTime / 1000000.f; // [s]
	const auto fixedFrameTime_ms = std::chrono::microseconds(fixedFrameTime);
	const auto fixedFrameTime_sc = std::chrono::duration_cast<std::chrono::steady_clock::duration>(fixedFrameTime_ms);

	RenderItemList renderItems;
	std::chrono::steady_clock::duration accumTime { 0 };
	std::chrono::steady_clock::duration sleepTime { 0 };
	std::chrono::steady_clock::duration elapsedTimeHistory[256] = { };
	uint frameIndex = 0;
	auto t0 = std::chrono::steady_clock::now();
	while (game.stateId != (int)GameStateId::quit)
	{
		ReloadScriptModule(aiModule);

		auto t1 = std::chrono::steady_clock::now();
		const auto elapsedTime = t1 - t0; // includes logic, rendering and sleeping
		t0 = t1;

		if (console.IsMinimized())
		{
			continue;
		}
		
		elapsedTimeHistory[frameIndex]  = elapsedTime;
		frameIndex = (frameIndex + 1) % std::size(elapsedTimeHistory);

		// Update logic and console at a fixed frame rate
		accumTime += elapsedTime;
		int maxIter = 2;
		while (accumTime > fixedFrameTime_sc)
		{ 
			accumTime -= fixedFrameTime_sc;
			if (maxIter > 0) // limit number of ticks in case accumTime is big (when debugging for example)
			{
				maxIter--;
				UpdateKeyStates();
				RunGameState(game, fixedDeltaTime);

				world.GetRenderItems(renderItems);
				console.Clear(Color::black);
				console.DrawSprites(renderItems.data(), (int)renderItems.size());
				console.DisplayMessages(messageLog);
				DrawGameState(game, console);
				console.DrawCanvas();

				messageLog.DeleteOldMessages(fixedDeltaTime, 3.f);
			}
		}

		// Adjust sleepTime so that elapsedTime converges towards the fixed frame time
		auto diff = fixedFrameTime_sc - elapsedTime;
		sleepTime += diff / 10;
		if (sleepTime.count() > 0)
		{
			std::this_thread::sleep_for(sleepTime);
		}
	}

#if 0
	// Calculate the mean iteratively to avoid overflows
	double avgElapsedTime = 0.;
	double i = 1.;
	for (auto t : elapsedTimeHistory)
	{
		avgElapsedTime += (t.count() - avgElapsedTime) / i;
		++i;
	}
	std::cout << "avgElapsedTime: " << avgElapsedTime / 1e6 << std::endl;
#endif

	return 0;
}


namespace
{

#define PARSE_INT(varName, minv, maxv) \
	if (!strcmp(name, #varName)) \
	{ \
		config.varName = std::max(minv, std::min(maxv, std::atoi(value))); \
		return 1; \
	}

#define PARSE_FLOAT(varName, minv, maxv) \
	do { \
		if (!strcmp(name, #varName)) \
		{ \
			config.varName = std::max(minv, std::min(maxv, (float)std::atof(value))); \
			return 1; \
		} \
	} while(0)

#define PARSE_BOOL(varName) \
	do { \
		if (!strcmp(name, #varName)) \
		{ \
			config.varName = strcmp(value, "true") ? false : true; \
			return 1; \
		} \
	} while(0)

int INIParser(void* user, [[maybe_unused]] const char* section, const char* name, const char* value)
{
	GameConfig& config = *(GameConfig*)user;
	PARSE_INT(consoleWidth, 1, 1000);
	PARSE_INT(consoleHeight, 1, 1000);
	PARSE_INT(fontSize, 2, 18);
	PARSE_FLOAT(playerFireRate, 0.f, 1000.f);
	PARSE_FLOAT(playerLaserVelocity, 0.f, 1000.f);
	PARSE_FLOAT(alienWaveSpeedInc, 0.f, 100.f);
	PARSE_FLOAT(alienWaveFireRateInc, 0.f, 100.f);
	PARSE_FLOAT(alienLaserVelocity, 0.f, 1000.f);
	PARSE_FLOAT(explosionTimer, 0.f, 100.f);
	PARSE_FLOAT(powerUpVelocity, 0.f, 100.f);
	PARSE_INT(powerUpHits, 0, 100);
	PARSE_FLOAT(powerUpFireBoost, 1.f, 10.f);
	PARSE_BOOL(godMode);
	return 1;
}


#undef PARSE_FLOAT
#undef PARSE_INT
#undef PARSE_BOOL


bool ReadGameConfig(GameConfig& config, const char* iniFileName)
{
	return 0 == ini_parse(iniFileName, INIParser, &config);
}


void RegisterGameStates(Game& game)
{
	RegisterStartMenu(game);
	RegisterGameState(game, &introScreenData, IntroScreen, DisplayIntroScreen, EnterIntroScreen, nullptr);
	RegisterGameState(game, &game, PlayGame, DisplayPlayGame, EnterPlayGame, nullptr);
	RegisterPauseScreen(game);
	RegisterGameState(game, nullptr, GameOverMenu, DisplayGameOver, nullptr, nullptr);
	RegisterGameState(game, nullptr, VictoryScreen, DisplayVictoryScreen, nullptr, nullptr);
	RegisterGameState(game, nullptr, nullptr, nullptr, nullptr, nullptr);
}

}
