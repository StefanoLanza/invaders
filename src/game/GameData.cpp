#include "GameData.h"
#include <engine/Timeline.h>
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <array> // std::size



namespace
{

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
const Animation boss0Anim = 
{
	{ GameImageId::boss0_0, GameImageId::boss0_1 }, 0.25f
};
const Animation boss1Anim = 
{
	{ GameImageId::boss1_0, GameImageId::boss1_1 }, 0.25f
};
const Animation boss2Anim = 
{
	{ GameImageId::boss2_0, GameImageId::boss2_1 }, 0.25f
};

const char* alienSeqNull = {
	""
};

const char* alienSeq0 = {
	"llrrllrrb"
	"llllrrrrb"
};

const char* alienSeq1 = {
	"rrllrrllb"
	"rrrrllllb"
};

const char* alienSeq2 = {
	"rrrrllllbllllrrrrb"
};

const char* alienSeq3 = {
	"llrrllrrb"
};

const char* alienSeq4 = {
	"brbrrblblbltlltrtrs"
};

const char* alienSeq5 = {
	"blbllbrbrbrtrrtltls"
};

const char* boss0Seq = {
	"brbrblblltltrtrt"
};

const char* boss1Seq = {
	"brbrblblltltrtrt"
};

const char* boss2Seq = {
	"brbrblblltltrtrt"
};

// TODO
//Action: move down wave

constexpr int oneHit = 1;
constexpr int twoHits = 2;
constexpr float fastFire = 0.4f;
constexpr float normalFire = 0.2f;
constexpr float boosFireRate = 1.f;

const AlienPrefab alienPrefabs[] =
{
	// Stage 0
	{ alien0Anim, Color::white, oneHit, alienSeq0, 10.f, 5.f, normalFire, },
	{ alien0Anim, Color::white, oneHit, alienSeq1, 10.f, 5.f, normalFire, },
	{ alien0Anim, Color::redIntense, twoHits, alienSeq0, 10.f, 5.f, fastFire, },
	{ alien0Anim, Color::redIntense, twoHits, alienSeq1, 10.f, 5.f, fastFire, },
	{ alien0Anim, Color::green, twoHits, alienSeq4, 12.f, 7.f, fastFire, },
	{ alien0Anim, Color::green, twoHits, alienSeq5, 12.f, 7.f, fastFire, },

	{ alien1Anim, Color::green, 1, alienSeq2, 10.f },
	{ alien2Anim, Color::blue, 1, alienSeq2, 10.f, },
	{ boss0Anim,  Color::violet, 8, boss0Seq, 20.f, 10.f, boosFireRate, },
	{ boss1Anim,  Color::violet, 10, boss1Seq, 20.f, 10.f, boosFireRate, },
	{ boss2Anim,  Color::violet, 12, boss2Seq, 20.f, 10.f, boosFireRate, },
};

const PlayerPrefab playerPrefabs[] =
{
	{ GameImageId::player, Color::white, Color::yellow, 30.f, 4.f },
	{ GameImageId::player, Color::yellow, Color::white, 30.f, 4.f },
};

const AlienWaveInfo stage0 =
{
	"00001111"
	"00001111"
	"00001111",
	8, 3,
	12.f, 4.f, 
	2.5
};

const AlienWaveInfo stage1 =
{
	"22223333"
	"00001111"
	"00001111",
	8, 3,
	12.f, 4.f, 
	2.5
};

const AlienWaveInfo stage2 =
{
	"22223333"
	"00001111"
	"00001111"
	" 4    5 ",
	8, 4,
	12.f, 4.f, 
	2.5
};

const AlienWaveInfo boss0 = 
{
	"7",
	1, 1,
	18.f, 16.f, 
	2.5
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

const Event level0Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage0, },
	{ GameEventId::message, 4.f, hudMessages[0], },
};

const Event level1Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage1, },
	{ GameEventId::message, 4.f, hudMessages[1], },
};

const Event level2Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage2, },
	{ GameEventId::message, 4.f, hudMessages[2], },
};

const Event level3Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &boss0, },
	{ GameEventId::message, 4.f, "Oh no, the boss!", },
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
	{ level1Events, static_cast<int>(std::size(level1Events)) },
	{ level2Events, static_cast<int>(std::size(level2Events)) },
	{ level3Events, static_cast<int>(std::size(level3Events)) },
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



const AlienPrefab& GetAlienPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(alienPrefabs));
	return alienPrefabs[index];
}

const PlayerPrefab& GetPlayerPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(playerPrefabs));
	return playerPrefabs[index];
}
