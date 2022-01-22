#include "GameData.h"
#include <engine/Timeline.h>
#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <array> // std::size


namespace
{

constexpr int screenWidth = 160;

enum Dir
{
	dir_n = 0,
	dir_nne,
	dir_ne,
	dir_ene,
	dir_e,
	dir_ese,
	dir_se,
	dir_sse,
	dir_s,
	dir_ssw,
	dir_sw,
	dir_wsw,
	dir_w,
	dir_wnw,
	dir_nw,
	dir_nnw,
};

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


constexpr PathEntry pathEnd = { -1, -1 };

// Paths
// s43 ssw3 sw3 wsw3 w3 wnw3 nw13
constexpr Path enterPath0
{
	.startx = screenWidth/2 - 5, 
	.starty = -7,
	.entries = {
		{dir_s, 13 },
		pathEnd
	},
};

// s43 sse3 se3 ese3 e3 ene3 ne13 
constexpr Path enterPath1
{
	.startx = screenWidth/2 + 5, 
	.starty = -7,
	.entries = {
		{dir_s, 13 },
		pathEnd
	},
};

// sw30 wsw3 w13 wnw3 nw6 
constexpr Path enterPath2
{
	.startx = screenWidth/2, 
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
constexpr Path enterPath3
{
	.startx = screenWidth/2, 
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

// Paths
constexpr Path enterPath4
{
	.startx = -5, 
	.starty = 0,
	.entries = {
		{dir_e, 10 },
		pathEnd
	},
};

constexpr Path enterPath5
{
	.startx = screenWidth + 5, 
	.starty = 0,
	.entries = {
		{dir_w, 10 },
		pathEnd
	},
};

const Path* enterPaths[] =
{
	&enterPath0,
	&enterPath1,
	&enterPath2,
	&enterPath3,
	&enterPath4,
	&enterPath5,
};

constexpr Path attackPath0
{
	.startx = 0, 
	.starty = 0,
	.entries = {
		 // llrrlllrrrb
		{ dir_w, 60 },
		{ dir_e, 60 },
		{ dir_w, 90 },
		{ dir_e, 90 },
		{ dir_s, 30 },
		pathEnd
	},
};

constexpr Path attackPath1
{
	.startx = 0, 
	.starty = 0,
	.entries = {
		// rrllrrrlllb
		{ dir_e, 60 },
		{ dir_w, 60 },
		{ dir_e, 90 },
		{ dir_w, 90 },
		{ dir_s, 30 },
		pathEnd
	},
};

constexpr Path attackPath2
{
	.startx = 0, 
	.starty = 0,
	.entries = {
		// lllllrrrrrb
		{ dir_w, 150 },
		{ dir_e, 150 },
		{ dir_s, 30 },
		// lllrrrllrrb
		{ dir_w, 90 },
		{ dir_e,  90 },
		{ dir_w,  60 },
		{ dir_e,  60 },
		{ dir_s,  30 },
		pathEnd
	},
};

constexpr Path attackPath3
{
	.startx = 0, 
	.starty = 0,
	.entries = {
		// rrrrrlllllb
		{ dir_e, 150 },
		{ dir_w, 150 },
		{ dir_s, 30 },
		// rrrlllrrllb
		{ dir_e, 90 },
		{ dir_w,  90 },
		{ dir_e,  60 },
		{ dir_w,  60 },
		{ dir_s,  30 },
		pathEnd
	},
};

// Left sentinel
constexpr Path attackPath4
{
	.startx = 0, 
	.starty = 0,
	.entries = {
		// brrblbllr
		{ dir_s, 30 },
		{ dir_e, 60 },
		{ dir_s, 30 },
		{ dir_w, 30 },
		{ dir_s, 30 },
		{ dir_w, 60 },
		{ dir_e, 30 },
		// lltrrtlrs
		{ dir_w, 60 },
		{ dir_n, 30 },
		{ dir_e, 60 },
		{ dir_n, 30 },
		{ dir_w, 30 },
		{ dir_e, 30 },
		//FIXME stop { dir_s, 30 },
		pathEnd
	},
};

// Right sentinel
constexpr Path attackPath5
{
	.startx = 0, 
	.starty = 0,
	.entries = {
		// bllbrbrrl
		{ dir_s, 30 },
		{ dir_w, 60 },
		{ dir_s, 30 },
		{ dir_e, 30 },
		{ dir_s, 30 },
		{ dir_e, 60 },
		{ dir_w, 30 },
		//  rrtlltrls
		{ dir_e, 60 },
		{ dir_n, 30 },
		{ dir_w, 60 },
		{ dir_n, 30 },
		{ dir_e, 30 },
		{ dir_w, 30 },
		//FIXME stop { dir_s, 30 },
		pathEnd
	},
};


constexpr Path boss0AttackPath
{
	.startx = 0, 
	.starty = 0,
	.entries = {
		// brrblrbll
		{ dir_s, 30 },
		{ dir_e, 60 },
		{ dir_s, 30 },
		{ dir_w, 30 },
		{ dir_e, 30 },
		{ dir_s, 30 },
		{ dir_w, 60 },
		// rlltlrr
		{ dir_e, 30 },
		{ dir_w, 60 },
		{ dir_n, 30 },
		{ dir_w, 30 },
		{ dir_e, 60 },
		// tlltrltrr
		{ dir_n, 30 },
		{ dir_w, 60 },
		{ dir_n, 30 },
		{ dir_e, 30 },
		{ dir_w, 30 },
		{ dir_e, 60 },
		// lrrl
		{ dir_w, 30 },
		{ dir_e, 60 },
		{ dir_w, 30 },
		pathEnd
	},
};

const Path* attackPaths[] =
{
	&attackPath0,
	&attackPath1,
	&attackPath2,
	&attackPath3,
	&attackPath4,
	&attackPath5,
	&boss0AttackPath,
};

// Animations
constexpr Animation alien0Anim = 
{
	.images	 = { GameImageId::alien0_0, GameImageId::alien0_1 }, 
	.duration = .5f,
};
constexpr Animation alien1Anim = 
{
	.images = { GameImageId::alien1_0, GameImageId::alien1_1 }, 
	.duration = .5f,
};
constexpr Animation alien2Anim = 
{
	.images = { GameImageId::alien2_0, GameImageId::alien2_1 }, 
	.duration = .5f,
};
constexpr Animation alien3Anim = 
{
	.images = { GameImageId::alien3_0, GameImageId::alien3_1 }, 
	.duration = .5f,
};
constexpr Animation alien4Anim = 
{
	.images = { GameImageId::alien4_0, GameImageId::alien4_1 }, 
	.duration = .5f,
};
constexpr Animation alien5Anim = 
{
	.images = { GameImageId::alien5_0, GameImageId::alien5_1 }, 
	.duration = .35f,
};
constexpr Animation boss0Anim = 
{
	.images = { GameImageId::boss0_0, GameImageId::boss0_1 }, 
	.duration = 0.25f,
};
constexpr Animation boss1Anim = 
{
	.images = { GameImageId::boss1_0, GameImageId::boss1_1 },
	 .duration = 0.25f,
};
constexpr Animation boss2Anim = 
{
	.images = { GameImageId::boss2_0, GameImageId::boss2_1 },
	.duration = 0.25f,
};


constexpr float enterSpeed = 100.f;
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

constexpr AlienPrefab alienPrefabs[] =
{
	// Stage 1,2,3,4 prefabs
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .speed= normalSpeed, .enterSpeed = enterSpeed,
	.fireRate = normalFire, .laserSpeed = laserSpeed, .aimAtPlayer =  doNotAim,   },
	{ .anim = alien0Anim, .color = Color::blue, .hits = 1, .speed= normalSpeed, .enterSpeed = enterSpeed, .fireRate = normalFire, .laserSpeed =  laserSpeed, .aimAtPlayer =  doNotAim,   },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .speed= normalSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,   },
	{ .anim = alien1Anim, .color = Color::red, .hits = 2, .speed= normalSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   doNotAim,   },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien2Anim, .color = Color::yellow, .hits = 3, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = boss0Anim,  .color = Color::violet, .hits = 10,.speed= 40.f,  .enterSpeed = enterSpeed, .fireRate = bossFireRate, .laserSpeed = fastLaserSpeed, .aimAtPlayer =  aim, },

	{ .anim = alien3Anim, .color = Color::blue, .hits = 1, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien3Anim, .color = Color::blue, .hits = 1, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = alien4Anim, .color = Color::red, .hits = 2, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien4Anim, .color = Color::red, .hits = 2, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = alien5Anim, .color = Color::yellow, .hits = 3, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =   aim,  },
	{ .anim = alien5Anim, .color = Color::yellow, .hits = 3, .speed= midSpeed, .enterSpeed = enterSpeed, .fireRate = fastFire, .laserSpeed = laserSpeed, .aimAtPlayer =  aim, },
	{ .anim = boss1Anim,  .color = Color::violet, .hits = 12, .speed = 42.f, .enterSpeed = enterSpeed, .fireRate = bossFireRate, .laserSpeed = fastLaserSpeed,},

//	{ boss2Anim,  Color::violet, 14, nullSeq, boss2Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed, },
};

constexpr PlayerPrefab playerPrefabs[] =
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
		110,100,90,80,80,90,100,110,
		70,60,50,40,40,50,60,70,
		30,20,10,0,0,10,20,30,
	},
	.enterPath = {
		4,4,4,4,5,5,5,5,
		4,4,4,4,5,5,5,5,
		4,4,4,4,5,5,5,5,
	},
	.attackPath = {
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

constexpr AlienWaveInfo wave2 =
{
	.mask = 
	"22223333"
	"00001111"
	"00001111",
	// Entering from above
	.enterDelay = {
		80,90,100,110,110,100,90,80,
		40,50,60,70,70,60,50,40,
		0,10,20,30,30,20,10,0,
	},
	.enterPath = {
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
	},
	.attackPath = {
		2,2,2,2,3,3,3,3,
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
	},
	.numCols = 8, 
	.numRows = 3,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};

constexpr AlienWaveInfo wave3 =
{
	.mask = 
		"22223333"
		"00001111"
		"00001111"
		"-4----5-",
	// Entering from above
	.enterDelay = {
		120,130,140,150,150,140,130,120,
		80,90,100,110,110,100,90,80,
		40,50,60,70,70,60,50,40,
		0,10,20,30,30,20,10,0,
	},
	.enterPath = {
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
	},
	.attackPath = {
		2,2,2,2,3,3,3,3,
		0,0,0,0,1,1,1,1,
		0,0,0,0,1,1,1,1,
		0,4,0,0,0,0,5,0,
	},
	.numCols = 8, 
	.numRows = 4,
	.dx = 12.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};

constexpr AlienWaveInfo wave4 = 
{
	.mask = 
		"2--6--3"
		"0-----1"
		"0-----1",
	// Entering from left, right and above
	.enterDelay = {
		20,0,0,0,0,0,20,
		10,0,0,0,0,0,10,
		 0,0,0,0,0,0, 0,
	},
	.enterPath = {
		2,0,0,0,0,0,3,
		2,0,0,0,0,0,3,
		2,0,0,0,0,0,3,
	},
	.attackPath = {
		2,0,0,6,0,0,3,
		0,0,0,0,0,0,1,
		0,0,0,0,0,0,1,
	},
	.numCols = 7, 
	.numRows = 3,
	.dx = 16.f, 
	.dy = 4.f, 
	.start_y = 4.0,
};

constexpr AlienWaveInfo wave5 =
{
	.mask = 
	"9999::::"
	"77778888"
	"77778888",
	.enterDelay = {
		110,100,90,80,80,90,100,110,
		70,60,50,40,40,50,60,70,
		30,20,10,0,0,10,20,30,
	},
	.enterPath = {
		4,4,4,4,5,5,5,5,
		4,4,4,4,5,5,5,5,
		4,4,4,4,5,5,5,5,
	},
	.attackPath = {
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

constexpr AlienWaveInfo testWave = 
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
	.enterPath = {
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

constexpr const char* const hudMessages[] =
{
	"Here they come!",
	"Destroy all aliens!",
	"Save the planet!",
	"Don't let them reach the bottom!",
	"Keep moving!",
};
constexpr int numHUDMessages = static_cast<int>(std::size(hudMessages));

constexpr Event stage1Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave1, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
	{ GameEventId::message, 4.f, hudMessages[0], },
};

constexpr Event stage2Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave2, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
	{ GameEventId::message, 4.f, hudMessages[1], },
};

constexpr Event stage3Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave3, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
	{ GameEventId::message, 4.f, hudMessages[2], },
};

constexpr Event stage4Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave4, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
	{ GameEventId::message, 4.f, "Boss", },
};

constexpr Event stage5Events[] =
{
	{ GameEventId::showStage, 0.f, nullptr },
	{ GameEventId::spawnWave, 0.f, &wave5, },
	{ GameEventId::hideScore, 0.f, nullptr, },
	{ GameEventId::hideStage, 1.f, nullptr },
	{ GameEventId::showScore, 2.f, nullptr, },
	{ GameEventId::message, 4.f, hudMessages[0], },
};


constexpr Stage stages[] =
{
	{ stage5Events, static_cast<int>(std::size(stage5Events)) },
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

const Path& GetEnterPath(int index) 
{
	return *enterPaths[index];
}

const Path& GetAttackPath(int index) 
{
	return *attackPaths[index];
}
