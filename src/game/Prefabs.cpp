#include "Prefabs.h"
#include "Images.h"
#include <array>
#include <cassert>
#include <cstdlib>


namespace
{

// Rules:
// it takes two laser shots to destroy a better alien ship.
const Animation alien0Anim = 
{
	{ ImageId::alien0_0, ImageId::alien0_1 }, .5f
};
const Animation alien1Anim = 
{
	{ ImageId::alien1_0, ImageId::alien1_1 }, .5f
};
const Animation alien2Anim = 
{
	{ ImageId::alien2_0, ImageId::alien2_1 }, .5f
};
const Animation alien3Anim = 
{
	{ ImageId::alien3_0, ImageId::alien3_1 }, .5f
};

const AlienPrefab alienPrefabs[] =
{
	{ alien0Anim, Color::white, 1, 20.f, },
	{ alien0Anim, Color::redIntense, 2, 40.f, },

	{ alien1Anim, Color::green, 1, 20.f, },
	{ alien1Anim, Color::redIntense, 2, 40.f, },

	{ alien2Anim, Color::blue, 1, 20.f, },
	{ alien2Anim, Color::redIntense, 2, 40.f, },

	{ alien3Anim, Color::blue, 1, 20.f, },
	{ alien3Anim, Color::redIntense, 2, 40.f, },
};


const AlienPrefab bossPrefabs[] =
{
	{ { { ImageId::boss0_0, ImageId::boss0_1 }, 0.25f }, Color::violet, 8, 40.f, },
	{ { { ImageId::boss1_0, ImageId::boss1_1 }, 0.25f }, Color::violet, 10, 40.f, },
	{ { { ImageId::boss2_0, ImageId::boss2_1 }, 0.25f }, Color::violet, 12, 40.f, },
};

}

const AlienPrefab& GetAlienPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(alienPrefabs));
	return alienPrefabs[index];
}

const AlienPrefab& GetBossPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(bossPrefabs));
	return bossPrefabs[index];
}


const PlayerPrefab playerPrefabs[] =
{
	{ ImageId::player, Color::white, Color::yellow, 30.f, 3.f },
	{ ImageId::player, Color::yellow, Color::white, 30.f, 3.f },
};

const PlayerPrefab& GetPlayerPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(playerPrefabs));
	return playerPrefabs[index];
}
