// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Game.h"
#include "Vector2D.h"
#include "GameConfig.h"
#include "PlayField.h"
#include "Player.h"
#include "Alien.h"
#include "Input.h"
#include "MessageLog.h"
#include "GameEvents.h"
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>
#include <cassert>


Game::Game(PlayField& world, const GameConfig& config, std::default_random_engine& rGen) : 
	world { world },
	config { config },
	rGen { rGen },
	state { State::start },
	score {}, 
	numPlayers { 0 },
	wallRow { 0 },
	accumTime { 0 },
	rndMessage { numHUDMessages, rGen }
{
}


int Game::GetScore(int playerIndex) const
{
	assert(playerIndex >= 0 && playerIndex < maxPlayers);
	return score[playerIndex];
}


int Game::GetNumPlayers() const
{
	return numPlayers;
}


void Game::Update(float dt, MessageLog& messageLog)
{
	if (state == State::start)
	{ 
		StartMenu();
	}
	else if (state == State::intro)
	{ 
		Intro(dt);
	}
	else if (state == State::running)
	{ 
		Run(dt, messageLog);
	}
	else if (state == State::paused)
	{ 
		PauseMenu();
	}
	else if (state == State::victory)
	{ 
		VictoryMenu(dt);
	}
	else if (state == State::over)
	{
		GameOverMenu();
	}
}


void Game::StartMenu()
{
	accumTime = 0;
	if (KeyJustPressed(KeyCode::_1))
	{
		Start(Mode::p1);
		state = State::intro;
	}
	else if (KeyJustPressed(KeyCode::_2))
	{
		Start(Mode::cpu1);
		state = State::intro;
	}
	else if (KeyJustPressed(KeyCode::_3))
	{
		Start(Mode::p1p2);
		state = State::intro;
	}
	else if (KeyJustPressed(KeyCode::_4))
	{
		Start(Mode::p1cpu2);
		state = State::intro;
	}
	else if (KeyJustPressed(KeyCode::_5))
	{
		Start(Mode::cpu1cpu2);
		state = State::intro;
	}
	else if (KeyJustPressed(KeyCode::escape))
	{
		state = State::quit;
	}
}


void Game::Intro(float dt)
{
	accumTime += dt;
	if (AnyKeyJustPressed() || accumTime > 8.f)
	{
		state = State::running;
	}
}

void Game::Run(float dt, MessageLog& messageLog)
{
	world.Update(dt, messageLog);
	if (world.GetPlayers().empty())
	{
		messageLog.Clear();
		state = State::over;
		return;
	}

	// Update score
	for (const auto& player : world.GetPlayers())
	{
		assert(player.GetId() < maxPlayers);
		score[player.GetId()] = player.GetScore();
	}

	if (eventIndex >= numGameEvents && world.NoAliens())
	{
		state = State::victory;
		return;
	}

	accumTime += dt;
	if (eventIndex < numGameEvents - 1 && accumTime > gameEvents[eventIndex].time)
	{
		++eventIndex;
		const Event& event = gameEvents[eventIndex];
		switch (event.type)
		{
			case EventType::randomMessage:
				{
					const int m = rndMessage.Next();
					messageLog.AddMessage(hudMessages[m], Color::yellowIntense);
				}
				break;
			case EventType::spawnWave:
				SpawnAlienWave(alienWaves[event.data]);
				break;
			case EventType::spawnWalls:
				SpawnWall(walls[event.data]);
				break;
			case EventType::boss:
				SpawnBoss(bossInfo[event.data]);
				break;
			default:
				break;
		}
	}

	if (KeyJustPressed(KeyCode::escape))
	{
		state = State::paused;
	}
}


void Game::PauseMenu()
{
	if (KeyJustPressed(KeyCode::escape))
	{
		state = State::start;
	}
	else if (KeyJustPressed(KeyCode::enter))
	{
		state = State::running;
	}
}


void Game::VictoryMenu(float dt)
{
	if (AnyKeyJustPressed())
	{
		state = State::start;
	}
}


void Game::GameOverMenu()
{
	if (AnyKeyJustPressed())
	{
		state = State::start;
	}
}


void Game::Start(Mode mode)
{
	if (config.randomSeed)
	{
		rGen.seed(config.randomSeed);
	}
	else
	{
		std::random_device random_device;
		rGen.seed( random_device() );
	}
	world.DestroyAll();
	for (int s = 0; s < maxPlayers; ++s)
	{
		score[s] = 0;
	}
	CreatePlayers(mode);
	world.Restart();
	std::uniform_int_distribution<int> rndInt { 0, 8 };
	wallRow = rndInt(rGen);
	rndMessage.Reset();
	eventIndex = 0;
	accumTime = 0.f;
}


void Game::CreatePlayers(Mode mode)
{
	const Vector2D worldBounds = world.GetBounds();
	std::unique_ptr<Input> input0, input1;
	if (mode == Mode::p1 || mode == Mode::p1cpu2 || mode == Mode::p1p2)
	{
		input0 = std::make_unique<KeyboardInput>(KeyCode::left, KeyCode::right);
	}
	else
	{
		input0 = std::make_unique<RndInput>(rGen);
	}
	if (mode == Mode::p1p2)
	{
		input1 = std::make_unique<KeyboardInput>(KeyCode::A, KeyCode::D);
	}
	else if (mode == Mode::p1cpu2 || mode == Mode::cpu1cpu2)
	{
		input1 = std::make_unique<RndInput>(rGen);
	}
	if (input1)
	{
		world.AddPlayerShip( PlayerShip { { worldBounds.x / 2 - 1, worldBounds.y - 1 }, config.playerVelocity, 0, std::move(input0), rGen } );
		world.AddPlayerShip( PlayerShip { { worldBounds.x / 2 + 1, worldBounds.y - 1 }, config.playerVelocity, 1, std::move(input1), rGen } );
		numPlayers = 2;
	}
	else
	{
		world.AddPlayerShip( PlayerShip { { worldBounds.x / 2, worldBounds.y - 1 }, config.playerVelocity, 0, std::move(input0), rGen } );
		numPlayers = 1;
	}
}


void Game::SpawnWall(const WallInfo& wall)
{
	float x = wall.x;
	const float y = wall.y;
	for (int k = 0; k < wall.n; k++)
	{
		world.AddWall( { x, y } );
		x += 1.f;
	}
	wallRow += 4;
}


void Game::SpawnBoss(const BossInfo& boss)
{

}


void Game::SpawnAlienWave(const AlienWave& wave)
{
	float x = (world.GetBounds().x - (wave.n * wave.dx)) / 2.f;
	float y = wave.y;
	for (int k = 0; k < wave.n; k++, x += wave.dx)
	{
		float direction = wave.direction;
		if (wave.split && k < wave.n / 2)
		{
			direction *= -1;
		}
		world.AddAlienShip( Alien { { x, y }, { direction * config.alienVelocity, config.alienDownVelocity }, config.alienHealth, rGen } );
	}
}

