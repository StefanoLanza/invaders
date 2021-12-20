#include "GameEvents.h"
#include "Prefabs.h"
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <array> // std::size


const AlienWave alienWavesLevel0[] =
{
	{ 2,  2.5, 16.f,  1.f, 0, 1, },
	{ 2,  2.5, 16.f, -1.f, 0, 1, },
	{ 4,  2.5, 16.f,  1.f, 0, 1, },
	{ 4,  2.5, 16.f, -1.f, 0, 1, },
	{ 6,  2.5, 16.f,  1.f, 0, 1, },
	{ 6,  2.5, 16.f, -1.f, 0, 1, },
};

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

const WallInfo walls[] =
{
	6, 40.f, 20.f,
	6, 100.f, 24.f,
	6, 60.f, 30.f,
};

const BossInfo bossInfo[] =
{
	{ 80, 8, 0/* boss id*/ },
	{ 80, 8, 1/* boss id*/ },
	{ 80, 8, 2/* boss id*/ },
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
	{ EventType::spawnWave, 6.f, &alienWavesLevel0[1], },
//	{ EventType::spawnWalls, 12.f, &walls[0], },
	{ EventType::spawnWave, 12.f, &alienWavesLevel0[2], },
//	{ EventType::spawnWalls, 24.f, &walls[1], },
	{ EventType::message, 18.f, hudMessages[1], },
	{ EventType::spawnWave, 18.f, &alienWavesLevel0[3], },
	{ EventType::message, 24.f, hudMessages[2], },
	{ EventType::spawnWave, 24.f, &alienWavesLevel0[4], },
	{ EventType::spawnWave, 30.f, &alienWavesLevel0[5], },
	{ EventType::boss, 42.f, &bossInfo[0], },
	{ EventType::message, 42.f, "Oh no, the boss!", },
};

const Event level1Events[] =
{
	{ EventType::wait, 2.f, nullptr },
	{ EventType::spawnWave, 0.f, &alienWavesLevel1[0], },
	{ EventType::message, 0.f, hudMessages[3], },
	{ EventType::spawnWave, 6.f, &alienWavesLevel1[1], },
//	{ EventType::spawnWalls, 12.f, &walls[0], },
	{ EventType::spawnWave, 12.f, &alienWavesLevel1[2], },
//	{ EventType::spawnWalls, 24.f, &walls[1], },
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

const Level levels[] =
{
	{ level0Events, static_cast<int>(std::size(level0Events)) },
	{ level1Events, static_cast<int>(std::size(level1Events)) },
	{ level2Events, static_cast<int>(std::size(level2Events)) }
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
