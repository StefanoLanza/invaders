// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <random>
#include <chrono>
#include <ctime>
#include <thread>
#include <cassert>
#include <iostream>

#include "Base.h"
#include "Vector2D.h"
#include "GameConfig.h"
#include "Game.h"
#include "Console.h"
#include "Renderer.h"
#include "PlayField.h"
#include "Input.h"
#include "MessageLog.h"
#include "DLL.h"


int main()
{
	DLL consoleDLL;
	Console console;
	if (!InitConsole(console, consoleDLL, "Win32Console.dll"))
	{
		return 0;
	}
	console.hideCursor(console.handle);

	// I changed the random engine from a global to a local variable. It is passed as reference to all classes that require it
	std::default_random_engine rGen;

	GameConfig gameConfig;
	// TODO Read game config from ini file

	const IVector2D consoleSize { gameConfig.worldWidth, gameConfig.worldHeight };
	const Vector2D worldSize { (float)gameConfig.worldWidth, (float)gameConfig.worldHeight };
	Renderer mainRenderer { consoleSize, console };
	if (! mainRenderer.InitializeConsole(gameConfig.fontSize))
	{
		return 0;
	}
	MessageLog messageLog;
	PlayField world { worldSize, gameConfig, rGen, messageLog };
	Game game { world, gameConfig, rGen, messageLog };

	// Simulation and rendering happen at a fixed rate
	const int fixedFrameTime = 16; // [ms]
	const float fixedDeltaTime = (float)fixedFrameTime / 1000.f; // [s]
	const auto fixedFrameTime_ms = std::chrono::milliseconds(fixedFrameTime);
	const auto fixedFrameTime_sc = std::chrono::duration_cast<std::chrono::steady_clock::duration>(fixedFrameTime_ms);

	RenderItemList rl;
	std::chrono::steady_clock::duration accumTime { 0 };
	std::chrono::steady_clock::duration sleepTime { 0 };
	std::chrono::steady_clock::duration elapsedTimeHistory[256] = { };
	uint frameIndex = 0;
	auto t0 = std::chrono::steady_clock::now();
	while (game.GetState() != GameStateId::quit)
	{
		ReloadDLL(consoleDLL);

		auto t1 = std::chrono::steady_clock::now();
		const auto elapsedTime = t1 - t0; // includes logic, rendering and sleeping
		t0 = t1;
		
		elapsedTimeHistory[frameIndex]  = elapsedTime;
		frameIndex = (frameIndex + 1) % _countof(elapsedTimeHistory);

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
				game.Update(fixedDeltaTime);
				world.GetRenderItems(rl);
				mainRenderer.Update(rl, game, messageLog);
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
	std::cout << "avgElapsedTime: " << avgElapsedTime << std::endl;

	return 0;
}
