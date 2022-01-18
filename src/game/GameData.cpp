#include "GameData.h"
#include <engine/Timeline.h>
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <array> // std::size


namespace
{

constexpr int screenWidth = 160;

constexpr const PathEntry peelLeft[] =
{
	{ 0, 3},
	{ 15,3},
	{ 14,3},
	{ 13,3},
	{ 12,3},
	{ 11,3},
	{ 10,3},
	{ 9,3},
	{ 8,3},
};

constexpr const PathEntry peelRight[] = 
{
	{ 0,3},
	{ 1,3},
	{ 2,3},
	{ 3,3},
	{ 4,3},
	{ 5,3},
	{ 6,3},
	{ 7,3},
	{ 8,3},
};

constexpr const PathEntry loop1[] =
  {
	{ 8,2},
	{ 9,2},
	{ 10,2},
	{ 11,2},
	{ 12,2},
	{ 13,2},
	{ 14,2},
	{ 15,2},
	{ 0,2},
	{ 1,2},
	{ 2,2},
	{ 3,2},
	{ 4,2},
	{ 5,2},
	{ 6,2},
	{ 7,2},
};

constexpr const PathEntry swoop1[] =
{
	{ 8,20},
	{ 9,3},
	{ 10,3},
	{ 11,3},
	{ 12,3},
};

constexpr const PathEntry swoop2[] =
{
	{ 8,20},
	{ 7,3},
	{ 6,3},
	{ 5,3},
	{ 4,3},
};

constexpr const PathEntry zigzag[] =
{
	{ 7,15 },
	{ 8,2 },
	{ 9,15 },
	{ 8,2 },
	{ 7,15 },
	{ 8,2 },
	{ 9,15 },
	{ 8,2 },
};

/*
0	CHKDIR("n");
1	CHKDIR("nne");
2	CHKDIR("ne");
3	CHKDIR("ene");
4	CHKDIR("e");
5	CHKDIR("ese");
6	CHKDIR("se");
7	CHKDIR("sse");
8	CHKDIR("s");
9	CHKDIR("ssw");
10	CHKDIR("sw");
11  CHKDIR("wsw");
12	CHKDIR("w");
13	CHKDIR("wnw");
14	CHKDIR("nw");
15	CHKDIR("nnw");
*/

constexpr PathEntry pathEnd = { -1, -1 };

// Paths
// s43 ssw3 sw3 wsw3 w3 wnw3 nw13
const Path path0
{
	.startx = 75, 
	.starty = -5,
	.entries = {
		{8, 43 },
		{9,  3 },
		{10,  3 },
		{11,  3 },
		{12,  3 },
		{13,  3 },
		{14,  13 },
		pathEnd
	},
};

// s43 sse3 se3 ese3 e3 ene3 ne13 
const Path path1
{
	.startx = 85, 
	.starty = -5,
	.entries = {
		{8, 43 },
		{7,  3 },
		{6,  3 },
		{5,  3 },
		{4,  3 },
		{3,  3 },
		{2,  13 },
		pathEnd
	},
};

// sw30 wsw3 w13 wnw3 nw6 
const Path path2
{
	.startx = 80, 
	.starty = -6,
	.entries = {
		{ 10, 30 }, 
		{ 11, 3 },
		{ 12, 13 },
		{ 13, 3 },
		{ 14, 6 },
		pathEnd
	},
};

// se30 ese3 e13 ene3 ne6 
const Path path3
{
	.startx = 80, 
	.starty = -6,
	.entries = {
		{ 6, 30 }, 
		{ 5, 3 },
		{ 4, 13 },
		{ 3, 3 },
		{ 2, 6 },
		pathEnd
	},
};

const Path* paths[] =
{
	&path0,
	&path1,
	&path2,
	&path3,
};

// Animations
const Animation alien0Anim = 
{
	.images	 = { GameImageId::alien0_0, GameImageId::alien0_1 }, 
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

constexpr const char* boss1Seq = {
	"brbrblblltltrtrt"
};

constexpr const char* boss2Seq = {
	"brbrblblltltrtrt"
};

constexpr float landingSpeed = 80.f;
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
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .attackSeq = alienSeq0, .speed= normalSpeed, .landingSpeed = landingSpeed,
	.fireRate = normalFire, .laserSpeed = laserSpeed, .aimAtPlayer =  doNotAim,   },
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .attackSeq = alienSeq1, .speed= normalSpeed, .landingSpeed = landingSpeed, .fireRate = normalFire, .laserSpeed =  laserSpeed, .aimAtPlayer =  doNotAim,   },
//	{ .anim = alien3Anim, .color = Color::violet, .hits = 4, .landingSeq = nullSeq, .attackSeq = alienSeq0, .speed= normalSpeed, .vspeed = downSpeed,
//	.fireRate = normalFire, .laserSpeed = laserSpeed, .aimAtPlayer =  doNotAim, .actionPlan = &testPlan },
//  { .anim = alien3Anim, .color = Color::violet, .hits = 4, .landingSeq = nullSeq, .attackSeq = alienSeq1, .speed= normalSpeed, .fireRate = normalFire, .laserSpeed =  laserSpeed, .aimAtPlayer =  doNotAim, },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .attackSeq = alienSeq4, .speed= normalSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,   },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .attackSeq = alienSeq5, .speed= normalSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,   },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .attackSeq = alienSeq2, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .attackSeq = alienSeq3, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = boss0Anim,  .color = Color::violet, .hits = 10,.attackSeq = boss0Pattern, .speed= 40.f,  .landingSpeed = landingSpeed, .fireRate = bossFireRate, .laserSpeed = fastLaserSpeed, .aimAtPlayer =  aim, },

	{ .anim = alien4Anim, .color = Color::yellow, .hits = 3, .attackSeq = alienSeq2, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien4Anim, .color = Color::yellow, .hits = 3, .attackSeq = alienSeq3, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = alien5Anim, .color = Color::yellow, .hits = 3, .attackSeq = alienSeq2, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien5Anim, .color = Color::yellow, .hits = 3, .attackSeq = alienSeq3, .speed= midSpeed, .landingSpeed = landingSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },

//	{ boss1Anim,  Color::violet, 12, nullSeq, boss1Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed,},

//	{ boss2Anim,  Color::violet, 14, nullSeq, boss2Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed, },
};

const PlayerPrefab playerPrefabs[] =
{
	{ .imageId = GameImageId::player1, .velocity= 30.f, .laserOffset = 4.f },
	{ .imageId = GameImageId::player2, .velocity= 30.f, .laserOffset = 4.f },
};


constexpr AlienWaveInfo wave1 =
{
	.mask = 
	"00001111"
	"00001111"
	"00001111",
	.enterDelay = {
		0,10,20,30,30,20,10,0,
		40,50,60,70,70,60,50,40,
		80,90,100,110,110,100,90,80,
	},
	.path = {
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
	},
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
	.enterDelay = {
		60,60,60,60,60,60,60,60,
		20,20,30,30,30,30,30,30,
		0,0,0,0,0,0,0,0,
	},
	.path = {
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
	},
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

const AlienWaveInfo testWave = 
{
	.mask = 
	"---0--0---"
	"--111111--"
	"1111111111"
	"1111111111"
	"1111111111"
	"1111111111",
	.enterDelay = {
		0,0, 0,435, 0,0,430,0,0,0,
		0,0, 425, 420, 415, 410, 405, 400, 0, 0,
345, 340, 335, 330, 325, 320, 315, 310, 305, 300,
245, 240, 235, 230, 225, 220, 215, 210, 205, 200,
145, 140, 135, 130, 125, 120, 115, 110, 105, 100,
050, 045, 040, 035, 030, 025, 020, 015, 010, 00,
	},
	.path = {
		0,0,0,0,0,0,1,0,0,0,
		0,0,0,0,0,1,1,1,1,1,
		0,0,0,0,0,1,1,1,1,1,
		0,0,0,0,0,1,1,1,1,1,
		0,0,0,0,0,1,1,1,1,1,
		0,0,0,0,0,1,1,1,1,1,
	},
	.numCols = 10, 
	.numRows = 6,
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
	{ GameEventId::spawnWave, 0.f, &wave1, },
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

int GetNumAlienPrefabs()
{
	return (int)std::size(alienPrefabs);
}


const PlayerPrefab& GetPlayerPrefab(int index)
{
	assert(index >= 0 && index < (int)std::size(playerPrefabs));
	return playerPrefabs[index];
}

const Path& GetPath(int index) 
{
	return *paths[index];
}
