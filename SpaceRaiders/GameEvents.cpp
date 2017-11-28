// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "GameEvents.h"
#include <cstdlib>



const AlienWave alienWaves[] =
{
	{ 4, 0.5, 4.f, 1.f, true },
	{ 4, 0.5, 4.f, -1.f, false  },
	{ 8, 0.5, 4.f, 1.f, true },
	{ 8, 0.5, 4.f, 1.f, true },
	{ 8, 0.5, 4.f, 1.f, false }
};

const WallInfo walls[] =
{
	6, 10.f, 10.f,
	6, 50.f, 14.f,
	6, 25.f, 6.f,
};

const BossInfo bossInfo[] =
{
	{ 10, 10 }
};

const Event gameEvents[] =
{
	{ EventType::spawnWave, 0.f, 0, },
	{ EventType::randomMessage, 0.f, 0, },
	{ EventType::spawnWave, 12.f, 1, },
	{ EventType::spawnWalls, 12.f, 0, },
	{ EventType::spawnWave, 24.f, 2, },
	{ EventType::spawnWalls, 24.f, 1, },
	{ EventType::randomMessage, 24.f, 0, },
	{ EventType::spawnWave, 36.f, 3, },
	{ EventType::boss, 48.f, 0, },
};
const int numGameEvents = _countof(gameEvents);

const char* hudMessages[] =
{
	"Here they come!",
	"Destroy All Aliens!",
	"Save the Ikans!",
	"Don't let them reach the bottom!",
	"Keep moving!"
};
const int numHUDMessages = _countof(hudMessages);
