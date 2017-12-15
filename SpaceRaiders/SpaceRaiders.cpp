// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <random>
#include <chrono>
#include <thread>
#include <cassert>

#include "Vector2D.h"
#include "GameConfig.h"
#include "Game.h"
#include "Renderer.h"
#include "PlayField.h"
#include "Input.h"
#include "MessageLog.h"


int main()
{
	// I changed the random engine from a global to a local variable. It is passed as reference to all classes that require it
	std::default_random_engine rGen;

	GameConfig gameConfig;
	// TODO Read game config from ini file

	const IVector2D consoleSize { gameConfig.worldWidth, gameConfig.worldHeight };
	const Vector2D worldSize { (float)gameConfig.worldWidth, (float)gameConfig.worldHeight };
	MessageLog messageLog;
	Renderer mainRenderer { consoleSize, gameConfig.fontSize };
	PlayField world { worldSize, gameConfig, rGen, messageLog };
	Game game { world, gameConfig, rGen, messageLog };

	// Simulation and rendering happen at a fixed rate
	const int fixedFrameTime = 33; // [ms]
	const float fixedDeltaTime = (float)fixedFrameTime / 1000.f;
	const auto updateTime = std::chrono::milliseconds(fixedFrameTime);

	RenderItemList rl;
	while (game.GetState() != Game::State::quit)
	{
		UpdateKeyStates();
		const auto start_time = std::chrono::steady_clock::now();
		game.Update(fixedDeltaTime);
		world.GetRenderItems(rl);
		mainRenderer.Update(rl, game, messageLog);
		messageLog.DeleteOldMessages(fixedDeltaTime, 3.f);
		const auto end_time = std::chrono::steady_clock::now();
		const auto deltaTime = end_time - start_time;
		// Sleep until the end of the frame time
		std::this_thread::sleep_for(updateTime - deltaTime);
	}

	return 0;
}
