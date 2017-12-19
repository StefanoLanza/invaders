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
#include "Prefabs.h"
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>
#include <cassert>


Game::Game(PlayField& world, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog_) :
	world { world },
	config { config },
	rGen { rGen },
	messageLog { messageLog_ },
	state { GameStateId::start },
	score {}, 
	numPlayers { 0 },
	wallRow { 0 },
	accumTime { 0 }
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


void Game::Update(float dt)
{
	state = RunGameState(state, *this, dt);
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
	world.Update(0.f);
	std::uniform_int_distribution<int> rndInt { 0, 8 };
	wallRow = rndInt(rGen);
	accumTime = 0.f;
	messageLog.Clear();
}


void Game::CreatePlayers(Mode mode)
{
	const Vector2D worldBounds = world.GetBounds();
	std::unique_ptr<Input> input0, input1;

	const PlayerPrefab& prefab0 = GetPlayerPrefab(0);
	const PlayerPrefab& prefab1 = GetPlayerPrefab(1);
	const Vector2D player0Size = GetImageSize(prefab0.imageId);
	const Vector2D player1Size = GetImageSize(prefab1.imageId);

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
		world.AddPlayerShip( PlayerShip { { worldBounds.x / 2 - player0Size.x, worldBounds.y - player0Size.y * 0.5f }, prefab0, 0, std::move(input0), rGen } );
		world.AddPlayerShip( PlayerShip { { worldBounds.x / 2 + player1Size.x, worldBounds.y - player1Size.y * 0.5f }, prefab1, 1, std::move(input1), rGen } );
		numPlayers = 2;
	}
	else
	{
		world.AddPlayerShip( PlayerShip { { worldBounds.x / 2, worldBounds.y - player0Size.y * 0.5f }, prefab0, 0, std::move(input0), rGen } );
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
		x += 4.f;
	}
	wallRow += 4;
}


void Game::SpawnBoss(const BossInfo& boss)
{
	const AlienPrefab& prefab = GetBossPrefab(boss.alienType);
	world.AddAlienShip( Alien_New( { boss.x, boss.y }, { prefab.speed, config.alienDownVelocity }, rGen, prefab, prefab ) );
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
		const AlienPrefab& alienPrefab = GetAlienPrefab(wave.alienType);
		const AlienPrefab& betterAlienPrefab = GetAlienPrefab(wave.betterAlienType);
		Vector2D velocity = { alienPrefab.speed * direction, config.alienDownVelocity };
		world.AddAlienShip( Alien_New( { x, y }, velocity, rGen, alienPrefab, betterAlienPrefab ) );
	}
}


void Game::ProcessEvent(const Event& event)
{
	switch (event.type)
	{
		case EventType::message:
			messageLog.AddMessage((const char*)event.data, Color::yellowIntense);
			break;
		case EventType::spawnWave:
			SpawnAlienWave(*(const AlienWave*)event.data);
			break;
		case EventType::spawnWalls:
			SpawnWall(*(WallInfo*)event.data);
			break;
		case EventType::boss:
			SpawnBoss(*(BossInfo*)event.data);
			break;
		case EventType::wait:
			break;
		default:
			break;
	};
}


void Game::NextLevel()
{
	++levelIndex;
	SetLevel(levelIndex);
}


void Game::SetLevel(int level)
{
	levelIndex = level;
	char tmp[64];
	sprintf_s(tmp, "Level: %d", levelIndex);
	messageLog.AddMessage(tmp, Color::yellowIntense);
	accumTime = 0.;
	world.DestroyWalls();
	eventIndex = 0;
}
