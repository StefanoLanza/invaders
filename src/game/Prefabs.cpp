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
	{ GetImageId(GameImageId::alien0_0), GetImageId(GameImageId::alien0_1) }, .5f
};
const Animation alien1Anim = 
{
	{ GetImageId(GameImageId::alien1_0), GetImageId(GameImageId::alien1_1) }, .5f
};
const Animation alien2Anim = 
{
	{ GetImageId(GameImageId::alien2_0), GetImageId(GameImageId::alien2_1) }, .5f
};
const Animation alien3Anim = 
{
	{ GetImageId(GameImageId::alien3_0), GetImageId(GameImageId::alien3_1) }, .5f
};

const AlienPrefab alienPrefabs[] =
{
	{ alien0Anim, Color::white, 1, 0, },
	{ alien0Anim, Color::redIntense, 2, 1, },

	{ alien1Anim, Color::green, 1, 0, },
	{ alien1Anim, Color::redIntense, 2, 1, },

	{ alien2Anim, Color::blue, 1, 0, },
	{ alien2Anim, Color::redIntense, 2, 1, },

	{ alien3Anim, Color::blue, 1, 0, },
	{ alien3Anim, Color::redIntense, 2, 1, },
};


const AlienPrefab bossPrefabs[] =
{
	{ { { GetImageId(GameImageId::boss0_0), GetImageId(GameImageId::boss0_1) }, 0.25f }, Color::violet, 8, 0, }, // FIXME BossScript, not 0
	{ { { GetImageId(GameImageId::boss1_0), GetImageId(GameImageId::boss1_1) }, 0.25f }, Color::violet, 10, 0, },
	{ { { GetImageId(GameImageId::boss2_0), GetImageId(GameImageId::boss2_1) }, 0.25f }, Color::violet, 12, 0, },
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
	{ GetImageId(GameImageId::player), Color::white, Color::yellow, 30.f, 4.f },
	{ GetImageId(GameImageId::player), Color::yellow, Color::white, 30.f, 4.f },
};

const PlayerPrefab& GetPlayerPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(playerPrefabs));
	return playerPrefabs[index];
}
