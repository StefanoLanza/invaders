#include "GameData.h"
#include <engine/Plan.h>
#include <engine/Timeline.h>
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <array> // std::size


namespace
{
// Animations
const Animation alien0Anim = 
{
	.images = { GameImageId::alien0_0, GameImageId::alien0_1 }, 
	.duration = .5f,
};
const Animation alien1Anim = 
{
	.images = { GameImageId::alien1_0, GameImageId::alien1_1 }, 
	.duration = .5f,
};
const Animation alien2Anim = 
{
	.images = { GameImageId::alien2_0, GameImageId::alien2_1 }, 
	.duration = .5f,
};
const Animation alien3Anim = 
{
	.images = { GameImageId::alien3_0, GameImageId::alien3_1 }, 
	.duration = .5f,
};
const Animation alien4Anim = 
{
	.images = { GameImageId::alien4_0, GameImageId::alien4_1 }, 
	.duration = .5f,
};
const Animation alien5Anim = 
{
	.images = { GameImageId::alien5_0, GameImageId::alien5_1 }, 
	.duration = .35f,
};
const Animation boss0Anim = 
{
	.images = { GameImageId::boss0_0, GameImageId::boss0_1 }, 
	.duration = 0.25f,
};
const Animation boss1Anim = 
{
	.images = { GameImageId::boss1_0, GameImageId::boss1_1 },
	 .duration = 0.25f,
};
const Animation boss2Anim = 
{
	.images = { GameImageId::boss2_0, GameImageId::boss2_1 },
	.duration = 0.25f,
};

// Action sequences
constexpr const char* circleSeq = {
	"llddbbeerrffttdd"
};


constexpr const char* nullSeq = {
	""
};

constexpr const char* alienSeq0 = {
	"lllrrrblllrrrb"
	"lllllrbrrrrllrrb"
};

constexpr const char* alienSeq1 = {
	"rrrlllbrrrlllb"
	"rrrrrlbllllrrllb"
};

constexpr const char* alienSeq4 = {
	"llrrlllrrrb"
};

constexpr const char* alienSeq5 = {
	"rrllrrrlllb"
};

// Sentinels
constexpr const char* alienSeq2 = {
	"brrblbllrlltrrtrls"
};

constexpr const char* alienSeq3 = {
	"bllbrbrrlrrtlltlrs"
};

constexpr const char* boss0Pattern = {
	"brrblrbll"
	"rlltlrrb"
	"tlltrltrr"
	"lrrl"
};

constexpr const char* landingPattern1 = {
	"rbl"
};

constexpr const char* landingPattern0 = {
	""//"lbr"
};

constexpr const char* boss1Seq = {
	"brbrblblltltrtrt"
};

constexpr const char* boss2Seq = {
	"brbrblblltltrtrt"
};

constexpr float landingSpeed = 272.f;
constexpr float normalSpeed = 12.f;
constexpr float midSpeed = 24.f;
constexpr float fastSpeed = 48.f;
constexpr float downSpeed = 10.f;
constexpr int oneHit = 1;
constexpr int twoHits = 2;
constexpr int threeHits = 3;
constexpr int fourHits = 4;
constexpr float normalFire = 0.1f;
constexpr float fastFire = 0.2f;
constexpr float bossFireRate = 1.f;
constexpr float laserSpeed = 30.f;
constexpr float midLaserSpeed = 35.f;
constexpr float fastLaserSpeed = 40.f;
constexpr bool aim = true;
constexpr bool doNotAim = false;

const AlienPrefab alienPrefabs[] =
{
	// Stage 1,2,3,4 prefabs
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .landingSeq = landingPattern0, .attackSeq = alienSeq0, .speed= normalSpeed, .landingSpeed = landingSpeed,
	.fireRate = normalFire, .laserSpeed = laserSpeed, .aimAtPlayer =  doNotAim,   },
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .landingSeq = landingPattern1, .attackSeq = alienSeq1, .speed= normalSpeed, .landingSpeed = landingSpeed, .fireRate = normalFire, .laserSpeed =  laserSpeed, .aimAtPlayer =  doNotAim,   },
//	{ .anim = alien3Anim, .color = Color::violet, .hits = 4, .landingSeq = nullSeq, .attackSeq = alienSeq0, .speed= normalSpeed, .vspeed = downSpeed,
//	.fireRate = normalFire, .laserSpeed = laserSpeed, .aimAtPlayer =  doNotAim, .actionPlan = &testPlan },
//  { .anim = alien3Anim, .color = Color::violet, .hits = 4, .landingSeq = nullSeq, .attackSeq = alienSeq1, .speed= normalSpeed, .fireRate = normalFire, .laserSpeed =  laserSpeed, .aimAtPlayer =  doNotAim, },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .landingSeq = landingPattern0, .attackSeq = alienSeq4, .speed= normalSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,   },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .landingSeq = landingPattern1, .attackSeq = alienSeq5, .speed= normalSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,   },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .landingSeq = landingPattern0, .attackSeq = alienSeq2, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .landingSeq = landingPattern1, .attackSeq = alienSeq3, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = boss0Anim,  .color = Color::violet, .hits = 10, .landingSeq = nullSeq, .attackSeq = boss0Pattern, .speed= 40.f,  .landingSpeed = landingSpeed, .fireRate = bossFireRate, .laserSpeed = fastLaserSpeed, .aimAtPlayer =  aim, },

	{ .anim = alien4Anim, .color = Color::yellow, .hits = 3, .landingSeq = nullSeq, .attackSeq = alienSeq2, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien4Anim, .color = Color::yellow, .hits = 3, .landingSeq = nullSeq, .attackSeq = alienSeq3, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = alien5Anim, .color = Color::yellow, .hits = 3, .landingSeq = nullSeq, .attackSeq = alienSeq2, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien5Anim, .color = Color::yellow, .hits = 3, .landingSeq = nullSeq, .attackSeq = alienSeq3, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },

//	{ boss1Anim,  Color::violet, 12, nullSeq, boss1Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed,},

//	{ boss2Anim,  Color::violet, 14, nullSeq, boss2Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed, },
};

const PlayerPrefab playerPrefabs[] =
{
	{ .imageId = GameImageId::player1, .velocity= 30.f, .laserOffset = 4.f },
	{ .imageId = GameImageId::player2, .velocity= 30.f, .laserOffset = 4.f },
};

constexpr AlienWaveInfo teststage =
{
	.mask = 
	"0",
	.numCols = 1, 
	.numRows = 1,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};


constexpr AlienWaveInfo wave1 =
{
	.mask = 
	"00001111"
	"00001111"
	"00001111",
	.numCols = 8, 
	.numRows = 3,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};

const AlienWaveInfo wave2 =
{
	.mask = 
	"22223333"
	"00001111"
	"00001111",
	.numCols = 8, 
	.numRows = 3,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};

const AlienWaveInfo wave3 =
{
	.mask = 
	"22223333"
	"00001111"
	"00001111"
	" 4    5 ",
	.numCols = 8, 
	.numRows = 4,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};

const AlienWaveInfo wave4 = 
{
	.mask = 
	"2  6  3"
	"0     1"
	"0     1",
	.numCols = 7, 
	.numRows = 3,
	.dx = 16.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};

const AlienWaveInfo testwave = 
{
	.mask = 
	"      3"
	"       "
	"       ",
	.numCols = 7, 
	.numRows = 3,
	.dx = 16.f, 
	.dy = 4.f, 
	.start_y = 4.0,
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

const Event stage1Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &testwave, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
//	{ GameEventId::message, 4.f, hudMessages[0], },
};

const Event stage2Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave2, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
//	{ GameEventId::message, 4.f, hudMessages[1], },
};

const Event stage3Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave3, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
//	{ GameEventId::message, 4.f, hudMessages[2], },
};

const Event stage4Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave4, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
//	{ GameEventId::message, 4.f, "Boss", },
};


constexpr Stage stages[] =
{
	{ stage1Events, static_cast<int>(std::size(stage1Events)) },
	{ stage2Events, static_cast<int>(std::size(stage2Events)) },
	{ stage3Events, static_cast<int>(std::size(stage3Events)) },
	{ stage4Events, static_cast<int>(std::size(stage4Events)) },
};

}

int GetNumStages()
{
	return static_cast<int>(std::size(stages));
}

const Stage& GetStage(int index)
{
	return stages[index];
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
