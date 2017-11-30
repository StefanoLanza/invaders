// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Prefabs.h"
#include "Images.h"
#include <cassert>
#include <cstdlib>


namespace
{

// Rules:
// it takes two laser shots to destroy a better alien ship.

const AlienPrefab alienPrefabs[] =
{
	{ { ImageId::alien0, Color::white }, 1, 20.f, },
	{ { ImageId::alien0, Color::redIntense }, 2, 40.f, },

	{ { ImageId::alien1, Color::green }, 1, 20.f, },
	{ { ImageId::alien1, Color::redIntense }, 2, 40.f, },

	{ { ImageId::alien2, Color::blue }, 1, 20.f, },
	{ { ImageId::alien2, Color::redIntense }, 2, 40.f, },

	{ { ImageId::alien3, Color::blue }, 1, 20.f, },
	{ { ImageId::alien3, Color::redIntense }, 2, 40.f, },
};


const AlienPrefab bossPrefabs[] =
{
	{ { ImageId::boss0, Color::green }, 8, 40.f, },
	{ { ImageId::boss1, Color::green }, 8, 40.f, },
	{ { ImageId::boss2, Color::green }, 8, 40.f, },
};

}

const AlienPrefab& GetAlienPrefab(int index)
{
	assert(index >= 0 && index < _countof(alienPrefabs));
	return alienPrefabs[index];
}

const AlienPrefab& GetBossPrefab(int index)
{
	assert(index >= 0 && index < _countof(bossPrefabs));
	return bossPrefabs[index];
}


const PlayerPrefab playerPrefabs[] =
{
	{ { ImageId::player, Color::white }, 30.f, 3.f },
	{ { ImageId::player, Color::yellow }, 30.f, 3.f },
};

const PlayerPrefab& GetPlayerPrefab(int index)
{
	assert(index >= 0 && index < _countof(playerPrefabs));
	return playerPrefabs[index];
}
