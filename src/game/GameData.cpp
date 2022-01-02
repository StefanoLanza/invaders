#include "GameData.h"
#include "Prefabs.h"
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <array> // std::size


const AlienSquad alienSquad0 = 
{
	8, 4,
	"11111111"
	"01111110"
	"00111100"
	"00011000",
};

const AlienSquad alienSquad1 = 
{
	8, 1,
	"01111110"
	"00111100"
	"00011000",
};

const AlienSquad alienSquad2 = 
{
	8, 1,
	"00111100"
	"00011000",
};

const AlienWaveInfo alienWavesLevel0[] =
{
	{ &alienSquad0, 12.f, 4.f, 2.5,  1.f, 10.f, /*.initialFireRate =*/ 0.5f, /*bounds=*/40},
};
/*
const AlienWave alienWavesLevel1[] =
{
	{ 2,  2.5, 16.f,  1.f, 2, 3, },
	{ 2,  2.5, 16.f, -1.f, 2, 3, },
	{ 4,  2.5, 16.f,  1.f, 2, 3, },
	{ 4,  2.5, 16.f, -1.f, 2, 3, },
	{ 6,  2.5, 16.f,  1.f, 2, 3, },
	{ 6,  2.5, 16.f, -1.f, 2, 3, },
};


const AlienWave alienWavesLevel2[] =
{
	{ 2,  4.5, 16.f,  1.f, 4, 5, },
	{ 2,  4.5, 16.f, -1.f, 4, 5, },
	{ 4,  4.5, 16.f,  1.f, 4, 5, },
	{ 4,  4.5, 16.f, -1.f, 4, 5, },
	{ 6,  4.5, 16.f, -1.f, 4, 5, },
	{ 6,  4.5, 16.f,  1.f, 4, 5, },
	{ 8,  4.5, 16.f, 1.f,  4, 5, },
	{ 8,  4.5, 16.f, -1.f, 4, 5, },
};
*/
const WallInfo walls[] =
{
	6, 40.f, 20.f,
	6, 100.f, 24.f,
	6, 60.f, 30.f,
};

const BossInfo bossInfo[] =
{
	{ 80, 8, 0/* boss id*/, 4.f, },
	{ 80, 8, 1/* boss id*/, 4.f },
	{ 80, 8, 2/* boss id*/, 4.f },
};

const char* const hudMessages[] =
{
	"Here they come!",
	"Destroy all aliens!",
	"Save the planet!",
	"Don't let them reach the bottom!",
	"Keep moving!",
};
constexpr int numHUDMessages = static_cast<int>(std::size(hudMessages));

namespace
{

const Event level0Events[] =
{
	{ EventType::wait, 2.f, nullptr },
	{ EventType::spawnWave, 0.f, &alienWavesLevel0[0], },
	{ EventType::message, 0.f, hudMessages[0], },
	//{ EventType::spawnWave, 12.f, &alienWavesLevel0[0], },
	//{ EventType::spawnWave, 12.f, &alienWavesLevel0[0], },
	{ EventType::message, 18.f, hudMessages[1], },
	//{ EventType::spawnWave, 18.f, &alienWavesLevel0[0], },
	{ EventType::message, 24.f, hudMessages[2], },
	//{ EventType::spawnWave, 24.f, &alienWavesLevel0[0], },
	//{ EventType::spawnWave, 30.f, &alienWavesLevel0[0], },
	//{ EventType::boss, 42.f, &bossInfo[0], },
	{ EventType::message, 42.f, "Oh no, the boss!", },
};
#if 0
const Event level1Events[] =
{
	{ EventType::wait, 2.f, nullptr },
	{ EventType::spawnWave, 0.f, &alienWavesLevel1[0], },
	{ EventType::message, 0.f, hudMessages[3], },
	{ EventType::spawnWave, 6.f, &alienWavesLevel1[1], },
	{ EventType::spawnWave, 12.f, &alienWavesLevel1[2], },
	{ EventType::message, 12.f, hudMessages[4], },
	{ EventType::spawnWave, 18.f, &alienWavesLevel1[3], },
	{ EventType::spawnWave, 24.f, &alienWavesLevel1[4], },
	{ EventType::message, 24.f, hudMessages[0], },
	{ EventType::spawnWave, 30.f, &alienWavesLevel1[5], },
	{ EventType::boss, 42.f, &bossInfo[1], },
	{ EventType::message, 42.f, "What a scary boss!", },
};

const Event level2Events[] =
{
	{ EventType::wait, 2.f, nullptr },
	{ EventType::spawnWave, 0.f, &alienWavesLevel2[0], },
	{ EventType::message, 0.f, hudMessages[1], },
	{ EventType::spawnWave, 6.f, &alienWavesLevel2[1], },
//	{ EventType::spawnWalls, 12.f, &walls[0], },
	{ EventType::spawnWave, 12.f, &alienWavesLevel2[2], },
	//{ EventType::spawnWalls, 24.f, &walls[1], },
	{ EventType::message, 18.f, hudMessages[2], },
	{ EventType::spawnWave, 18.f, &alienWavesLevel2[3], },
	{ EventType::spawnWave, 24.f, &alienWavesLevel2[4], },
	{ EventType::message, 30.f, hudMessages[3], },
	{ EventType::spawnWave, 30.f, &alienWavesLevel2[5], },
	{ EventType::spawnWave, 36.f, &alienWavesLevel2[6], },
	{ EventType::spawnWave, 42.f, &alienWavesLevel2[7], },
	{ EventType::boss, 54.f, &bossInfo[2], },
	{ EventType::message, 54.f, "Kill the final boss!", },
};
#endif

const Level levels[] =
{
	{ level0Events, static_cast<int>(std::size(level0Events)) },
	//{ level1Events, static_cast<int>(std::size(level1Events)) },
	//{ level2Events, static_cast<int>(std::size(level2Events)) },
	// TODO Add at least 2 levels
};

}

int GetNumLevels()
{
	return static_cast<int>(std::size(levels));
}

const Level& GetLevel(int index)
{
	return levels[index];
}



namespace
{

// Rules:
// it takes two laser shots to destroy a better alien ship.
const Animation alien0Anim = 
{
	{ GameImageId::alien0_0, GameImageId::alien0_1 }, .5f
};
const Animation alien1Anim = 
{
	{ GameImageId::alien1_0, GameImageId::alien1_1 }, .5f
};
const Animation alien2Anim = 
{
	{ GameImageId::alien2_0, GameImageId::alien2_1 }, .5f
};
const Animation alien3Anim = 
{
	{ GameImageId::alien3_0, GameImageId::alien3_1 }, .5f
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
	{ { { GameImageId::boss0_0, GameImageId::boss0_1 }, 0.25f }, Color::violet, 8, 0, }, // FIXME BossScript, not 0
	{ { { GameImageId::boss1_0, GameImageId::boss1_1 }, 0.25f }, Color::violet, 10, 0, },
	{ { { GameImageId::boss2_0, GameImageId::boss2_1 }, 0.25f }, Color::violet, 12, 0, },
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
	{ GameImageId::player, Color::white, Color::yellow, 30.f, 4.f },
	{ GameImageId::player, Color::yellow, Color::white, 30.f, 4.f },
};

const PlayerPrefab& GetPlayerPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(playerPrefabs));
	return playerPrefabs[index];
}
