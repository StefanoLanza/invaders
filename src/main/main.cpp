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
#include <engine/Input.h>
#include <engine/MessageLog.h>
#include <engine/ScriptModule.h>
#include <engine/Console.h>
#include <engine/DLL.h>
#include <engine/Path.h>
#include <inih-master/ini.h>

#include <game/Images.h>
#include <game/GameConfig.h>
#include <game/Game.h>
#include <game/PlayField.h>
#include <game/GameStates.h>
#include <game/StartMenu.h>
#include <game/PauseScreen.h>
#include <game/PlayGameState.h>
#include <game/GameOverState.h>
#include <game/IntroScreen.h>
#include <game/VictoryScreen.h>

namespace
{
	
void ReadGameConfig(GameConfig& config, const char* iniFile);
void RegisterGameStates(Game& game);
void DisplayScores(const Game& game, Renderer& renderer);

}


int main()
{
	std::cout << "IKA Invaders" << std::endl;
	std::cout << "by Stefano Lanza - steflanz@gmail.com - 2021" << std::endl;
	std::cout << std::endl;

	//char exePath[MAX_PATH];
	//getExePath(exePath, sizeof exePath);
	//cutPath(exePath);

	std::cout << "Reading game config from game.ini" << std::endl;
	GameConfig gameConfig;
	ReadGameConfig(gameConfig, "game.ini");

	ScriptModule scriptModule;
    //char tmp[MAX_PATH];
	//if (snprintf(tmp, sizeof tmp, "%s%cScripts.dll", exePath, DIR_SEPARATOR) > (int)(sizeof tmp)) {
	//	return -1;
	//}
	const char* scriptsFileName = 
	#ifdef WINDOWS
		"Scripts.dll";
	#else
		"libScripts.so";
	#endif
	if (! InitScriptModule(scriptModule, scriptsFileName))
	{
		std::cerr << "Cannot initialize DLL" << std::endl;
		return -1;
	}

	const Vector2D worldSize { (float)gameConfig.worldWidth, (float)gameConfig.worldHeight };
	Renderer renderer;
	if (! renderer.Initialize(gameConfig.worldWidth, gameConfig.worldHeight, gameConfig.fontSize))
	{
		std::cerr << "Cannot initialize console" << std::endl;
		return -1;
	}

	std::default_random_engine rGen;
	MessageLog messageLog;
	PlayField world { worldSize, gameConfig, rGen, messageLog };
	Game game = NewGame(world, gameConfig, rGen, messageLog, scriptModule);
	RegisterGameStates(game);

	InitGameImages();

	EnterGameState(game, (int)GameStateId::start);

	// Simulation and rendering happen at a fixed rate
	const int fixedFrameTime = 16; // [ms]
	const float fixedDeltaTime = (float)fixedFrameTime / 1000.f; // [s]
	const auto fixedFrameTime_ms = std::chrono::milliseconds(fixedFrameTime);
	const auto fixedFrameTime_sc = std::chrono::duration_cast<std::chrono::steady_clock::duration>(fixedFrameTime_ms);

	RenderItemList renderItems;
	std::chrono::steady_clock::duration accumTime { 0 };
	std::chrono::steady_clock::duration sleepTime { 0 };
	std::chrono::steady_clock::duration elapsedTimeHistory[256] = { };
	uint frameIndex = 0;
	auto t0 = std::chrono::steady_clock::now();
	while (game.stateId != (int)GameStateId::quit)
	{
		//ReloadDLL(consoleDLL);
		ReloadScriptModule(scriptModule);

		auto t1 = std::chrono::steady_clock::now();
		const auto elapsedTime = t1 - t0; // includes logic, rendering and sleeping
		t0 = t1;
		
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
				renderer.Clear(Color::black);
				renderer.DrawSprites(renderItems.data(), (int)renderItems.size());
				renderer.DisplayMessages(messageLog);
				DisplayScores(game, renderer);
				DrawGameState(game, renderer);
				renderer.DrawCanvas();

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

	// Calculate the mean iteratively to avoid overflows
	double avgElapsedTime = 0.;
	double i = 1.;
	for (auto t : elapsedTimeHistory)
	{
		avgElapsedTime += (t.count() - avgElapsedTime) / i;
		++i;
	}
//	std::cout << "avgElapsedTime: " << avgElapsedTime << std::endl;

	return 0;
}


namespace
{

#define PARSE_INT(varName, minv, maxv) \
	if (!strcmp(name, #varName)) \
	{ \
		config.varName = std::max(minv, std::min(maxv, std::atoi(value))); \
	}

#define PARSE_FLOAT(varName, minv, maxv) \
	do { \
		if (!strcmp(name, #varName)) \
		{ \
			config.varName = std::max(minv, std::min(maxv, (float)std::atof(value))); \
		} \
	} while(0)

#define PARSE_BOOL(varName) \
	if (!strcmp(name, #varName)) \
	{ \
		config.varName = strcmp(value, "true") ? false : true; \
	}

int INIParser(void* user, [[maybe_unused]] const char* section, const char* name, const char* value)
{
	GameConfig& config = *(GameConfig*)user;
	PARSE_INT(maxAlienLasers, 0, 100);
	PARSE_INT(maxPlayerLasers, 0, 100);
	PARSE_FLOAT(playerFireRate, 0.f, 1000.f);
	PARSE_FLOAT(playerLaserVelocity, 0.f, 1000.f);
	PARSE_BOOL(godMode);
	PARSE_FLOAT(alienSpeedMul, 0.f, 100.f);
	PARSE_FLOAT(alienLaserVelocity, 0.f, 1000.f);
	PARSE_FLOAT(alienTransformEnergy, 0.f, 1000.f);
	PARSE_FLOAT(alienDownVelocity, 0.f, 1000.f);
	PARSE_FLOAT(alienFireRate, 0.f, 1000.f);
	PARSE_FLOAT(betterAlienFireRate, 0.f, 1000.f);
	PARSE_FLOAT(explosionTimer, 0.f, 100.f);
	PARSE_FLOAT(powerUpVelocity, 0.f, 100.f);
	PARSE_INT(powerUpHits, 0, 100);
	PARSE_FLOAT(powerUpFireBoost, 1.f, 10.f);
	return 1;
}
#undef PARSE_FLOAT
#undef PARSE_INT
#undef PARSE_BOOL


void ReadGameConfig(GameConfig& config, const char* iniFileName)
{
	ini_parse(iniFileName, INIParser, &config);
}


void RegisterGameStates(Game& game)
{
	RegisterGameState(game, &startMenuData, StartMenu, DisplayStartMenu, EnterStartMenu);
	RegisterGameState(game, &introScreenData, IntroScreen, DisplayIntroScreen, EnterIntroScreen);
	RegisterGameState(game, &playGameStateData, PlayGame, DisplayPlayGame, EnterPlayGame);
	RegisterGameState(game, nullptr, PauseScreen, DisplayPauseScreen, nullptr);
	RegisterGameState(game, nullptr, GameOverMenu, DisplayGameOver, nullptr);
	RegisterGameState(game, nullptr, VictoryScreen, DisplayVictoryScreen, nullptr);
	RegisterGameState(game, nullptr, nullptr, nullptr, nullptr);
}

void DisplayScores(const Game& game, Renderer& renderer) {
	char tmp[256];
	for (int p = 0; p < game.numPlayers; ++p)
	{
		snprintf(tmp, sizeof tmp - 1, "P%d Score: %d", p + 1, game.score[p]);
		tmp[sizeof tmp - 1] = 0;
		renderer.DisplayText(tmp, 0, p, Color::white);
	}
}

}
