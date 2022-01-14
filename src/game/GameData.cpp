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

// Sentinels
constexpr const char* alienSeq2 = {
	"brbrblblbltrlltrrtrls"
};

constexpr const char* alienSeq3 = {
	"blblbrbrbrtlrrtlltlrs"
};

constexpr const char* boss0Seq = {
	"brrbrlbllrlltlrrbtlltlrtrrlrrrll"
};

constexpr const char* boss1Seq = {
	"brbrblblltltrtrt"
};

constexpr const char* boss2Seq = {
	"brbrblblltltrtrt"
};

constexpr float normalSpeed = 12.f;
constexpr float midSpeed = 24.f;
constexpr float fastSpeed = 48.f;
constexpr float downSpeed = 10.f;
constexpr int oneHit = 1;
constexpr int twoHits = 2;
constexpr int threeHits = 3;
constexpr int fourHits = 4;
constexpr float fastFire = 0.4f;
constexpr float normalFire = 0.2f;
constexpr float bossFireRate = 1.f;
constexpr float laserSpeed = 30.f;
constexpr float midLaserSpeed = 35.f;
constexpr float fastLaserSpeed = 40.f;
constexpr bool aim = true;
constexpr bool doNotAim = false;

constexpr Action testPlanActions[] =
{
//	{ .id = ActionId::path, .path = { .seq = alienSeq0, .vx = 16.f, .vy = 4.f, .duration = 30 } },
	{ .id = ActionId::translate, .translate = { .dx = 32, .dy = 0, .duration = 1 } },
	{ .id = ActionId::rotate, .rotate = { .dtheta = -2160.f, .duration = 2040 } },
//	{ .id = ActionId::move, .move = { .x = 80, .y = 27, .speed = 16.f } },
//	{ .id = ActionId::translate, .translate = { .dx = -16, .dy = 0, .duration = 20 } },
//	{ .id = ActionId::rotate, .rotate = { .dtheta = 0.02f, .duration = 240 } },
//	{ .id = ActionId::translate, .translate = { .dx = -16, .dy = 0, .duration = 120 } },
//	{ .id = ActionId::translate, .translate = { .dx = +16, .dy = 0, .duration = 120 } },
//	{ .id = ActionId::translate, .translate = { .dx = 0, .dy = -16, .duration = 120 } },
};
constexpr Plan testPlan = 
{
	.actions = testPlanActions,
	.actionCount = (int)std::size(testPlanActions),
	.loop = true,
};

const AlienPrefab alienPrefabs[] =
{
	// Stage 1,2,3,4 prefabs
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .landingSeq = nullSeq, .actionSeq = alienSeq0, .speed= normalSpeed, 
	.fireRate = normalFire, .laserSpeed = laserSpeed, .aimAtPlayer =  doNotAim, .actionPlan = &testPlan },
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .landingSeq = nullSeq, .actionSeq = alienSeq1, .speed= normalSpeed, .fireRate = normalFire, .laserSpeed =  laserSpeed, .aimAtPlayer =  doNotAim, },
//	{ .anim = alien3Anim, .color = Color::violet, .hits = 4, .landingSeq = nullSeq, .actionSeq = alienSeq0, .speed= normalSpeed, .vspeed = downSpeed,
//	.fireRate = normalFire, .laserSpeed = laserSpeed, .aimAtPlayer =  doNotAim, .actionPlan = &testPlan },
//  { .anim = alien3Anim, .color = Color::violet, .hits = 4, .landingSeq = nullSeq, .actionSeq = alienSeq1, .speed= normalSpeed, .fireRate = normalFire, .laserSpeed =  laserSpeed, .aimAtPlayer =  doNotAim, },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .landingSeq = nullSeq, .actionSeq = alienSeq0, .speed= normalSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,   },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .landingSeq = nullSeq, .actionSeq = alienSeq1, .speed= normalSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,  },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .landingSeq = nullSeq, .actionSeq = alienSeq2, .speed= midSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .landingSeq = nullSeq, .actionSeq = alienSeq3, .speed= midSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = boss0Anim,  .color = Color::violet, .hits = 10, .landingSeq = nullSeq, .actionSeq = boss0Seq, .speed= 40.f,  .fireRate = bossFireRate, .laserSpeed = fastLaserSpeed, .aimAtPlayer =  aim, },

//	{ boss1Anim,  Color::violet, 12, nullSeq, boss1Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed,},

//	{ boss2Anim,  Color::violet, 14, nullSeq, boss2Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed, },
};

const PlayerPrefab playerPrefabs[] =
{
	{ .imageId = GameImageId::player1, .velocity= 30.f, .laserOffset = 4.f },
	{ .imageId = GameImageId::player2, .velocity= 30.f, .laserOffset = 4.f },
};

const AlienWaveInfo teststage =
{
	.mask = 
	"0",
	.numCols = 1, 
	.numRows = 1,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 3.5,
};


const AlienWaveInfo stage1 =
{
	.mask = 
	"00001111"
	"00001111"
	"00001111",
	.numCols = 8, 
	.numRows = 3,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 3.5,
};

const AlienWaveInfo stage2 =
{
	"22223333"
	"00001111"
	"00001111",
	8, 3,
	12.f, 4.f, 
	3.5
};

const AlienWaveInfo stage3 =
{
	"22223333"
	"00001111"
	"00001111"
	" 4    5 ",
	8, 4,
	12.f, 4.f, 
	3.5
};

const AlienWaveInfo stage4 = 
{
	"0  6  1"
	"0     1"
	"0     1",
	7, 3,
	16.f, 
	4.f, 
	3.5
};

const AlienWaveInfo stage5 =
{
	"77778888"
	"77778888"
	"77778888",
	8, 3,
	12.f, 4.f, 
	2.5
};

const AlienWaveInfo stage6 =
{
	"9999::::"
	"77778888"
	"77778888",
	8, 3,
	12.f, 4.f, 
	2.5
};

const AlienWaveInfo stage7 =
{
	"9999::::"
	"77778888"
	"77778888"
	" ;    < ",
	8, 4,
	12.f, 4.f, 
	2.5
};

// Boss 
const AlienWaveInfo stage8 = 
{
	"=",
	1, 1,
	0.f, 
	0.f, 
	2.5
};

const AlienWaveInfo stage12 = 
{
	"20",//FIXME
	1, 1,
	0.f, 
	0.f, 
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

const Event stage1Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &teststage }, //&stage1, },
	{ GameEventId::message, 4.f, hudMessages[0], },
};

const Event stage2Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage2, },
	{ GameEventId::message, 4.f, hudMessages[1], },
};

const Event stage3Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage3, },
	{ GameEventId::message, 4.f, hudMessages[2], },
};

const Event stage4Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage4, },
	{ GameEventId::message, 4.f, "Boss", },
};

const Event stage5Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage5, },
	{ GameEventId::message, 4.f, hudMessages[3], },
};

const Event stage6Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage6, },
	{ GameEventId::message, 4.f, hudMessages[4], },
};

const Event stage7Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage7, },
	{ GameEventId::message, 4.f, hudMessages[0], },
};

const Event stage8Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::hideStage, 2.f, nullptr },
	{ GameEventId::spawnWave, 2.f, &stage8, },
	{ GameEventId::message, 4.f, "Boss", },
};

const Stage stages[] =
{
	{ stage1Events, static_cast<int>(std::size(stage1Events)) },
	{ stage2Events, static_cast<int>(std::size(stage2Events)) },
	{ stage3Events, static_cast<int>(std::size(stage3Events)) },
	{ stage4Events, static_cast<int>(std::size(stage4Events)) },
	{ stage5Events, static_cast<int>(std::size(stage5Events)) },
	{ stage6Events, static_cast<int>(std::size(stage6Events)) },
	{ stage7Events, static_cast<int>(std::size(stage7Events)) },
	{ stage8Events, static_cast<int>(std::size(stage8Events)) },
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
