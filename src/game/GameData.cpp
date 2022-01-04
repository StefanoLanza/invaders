#include "GameData.h"
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

// Action sequences
const char* alienSeqNull = {
	""
};

const char* alienSeq0 = {
	"lllrrrblllrrrb"
	"lllllrbrrrrllrrb"
};

const char* alienSeq1 = {
	"rrrlllbrrrlllb"
	"rrrrrlbllllrrllb"
};

// Sentinels
const char* alienSeq2 = {
	"brbrblblbltrlltrrtrls"
};

const char* alienSeq3 = {
	"blblbrbrbrtlrrtlltlrs"
};

const char* boss0Seq = {
	"brrbrlbllrlltlrrbtlltlrtrrlrrbrll"
};

const char* boss1Seq = {
	"brbrblblltltrtrt"
};

const char* boss2Seq = {
	"brbrblblltltrtrt"
};

constexpr float normalSpeed = 12.f;
constexpr float midSpeed = 24.f;
constexpr float fastSpeed = 48.f;
constexpr float downSpeed = 5.f;
constexpr int oneHit = 1;
constexpr int twoHits = 2;
constexpr int threeHits = 3;
constexpr float fastFire = 0.4f;
constexpr float normalFire = 0.2f;
constexpr float bossFireRate = 2.f;
constexpr float laserSpeed = 30.f;
constexpr float midLaserSpeed = 35.f;
constexpr float fastLaserSpeed = 45.f;
constexpr float veryFastLaserSpeed = 60.f;

const AlienPrefab alienPrefabs[] =
{
	// Stage 1
	{ alien0Anim, Color::white, oneHit, alienSeq0, normalSpeed, downSpeed, normalFire, laserSpeed, },
	{ alien0Anim, Color::white, oneHit, alienSeq1, normalSpeed, downSpeed, normalFire,  laserSpeed, },
	{ alien0Anim, Color::redIntense, twoHits, alienSeq0, normalSpeed, downSpeed, fastFire, laserSpeed,  },
	{ alien0Anim, Color::redIntense, twoHits, alienSeq1, normalSpeed, downSpeed, fastFire, laserSpeed,  },
	{ alien0Anim, Color::yellow, threeHits, alienSeq2, midSpeed, 7.f, fastFire, laserSpeed,  },
	{ alien0Anim, Color::yellow, threeHits, alienSeq3, midSpeed, 7.f, fastFire, laserSpeed,  },
	{ boss0Anim,  Color::violet, 10, boss0Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed, },

	// Stage 2
	{ alien1Anim, Color::white, oneHit, alienSeq0, normalSpeed, downSpeed, normalFire, midLaserSpeed, },
	{ alien1Anim, Color::white, oneHit, alienSeq1, normalSpeed, downSpeed, normalFire,  midLaserSpeed, },
	{ alien1Anim, Color::redIntense, twoHits, alienSeq0, normalSpeed, downSpeed, fastFire, midLaserSpeed,  },
	{ alien1Anim, Color::redIntense, twoHits, alienSeq1, normalSpeed, downSpeed, fastFire, midLaserSpeed,  },
	{ alien1Anim, Color::yellow, threeHits, alienSeq2, midSpeed, 7.f, fastFire, midLaserSpeed,  },
	{ alien1Anim, Color::yellow, threeHits, alienSeq3, midSpeed, 7.f, fastFire, midLaserSpeed,  },
	{ boss1Anim,  Color::violet, 12, boss1Seq, 40.f, 20.f, bossFireRate, fastLaserSpeed,},

	// Stage 3
	{ alien2Anim, Color::white, oneHit, alienSeq0, midSpeed, downSpeed, normalFire, midLaserSpeed, },
	{ alien2Anim, Color::white, oneHit, alienSeq1, midSpeed, downSpeed, normalFire,  midLaserSpeed, },
	{ alien2Anim, Color::redIntense, twoHits, alienSeq0, midSpeed, downSpeed, fastFire, midLaserSpeed,  },
	{ alien2Anim, Color::redIntense, twoHits, alienSeq1, midSpeed, downSpeed, fastFire, midLaserSpeed,  },
	{ alien2Anim, Color::yellow, threeHits, alienSeq2, midSpeed, 7.f, fastFire, midLaserSpeed,  },
	{ alien2Anim, Color::yellow, threeHits, alienSeq3, midSpeed, 7.f, fastFire, midLaserSpeed,  },
	{ boss2Anim,  Color::violet, 14, boss2Seq, 40.f, 20.f, bossFireRate, veryFastLaserSpeed, },
};

const PlayerPrefab playerPrefabs[] =
{
	{ GameImageId::player, Color::white, Color::yellow, 30.f, 4.f },
	{ GameImageId::player, Color::yellow, Color::white, 30.f, 4.f },
};

const AlienWaveInfo stage1 =
{
	"00001111"
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
	"00001111",
	8, 3,
	12.f, 4.f, 
	2.5
};

const AlienWaveInfo stage3 =
{
	"22223333"
	"00001111"
	"00001111"
	" 4    5 ",
	8, 4,
	12.f, 4.f, 
	2.5
};

const AlienWaveInfo stage4 = 
{
	"6",
	1, 1,
	0.f, 
	0.f, 
	2.5
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
	{ GameEventId::spawnWave, 2.f, &stage1, },
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
	return stages[index + 4]; //FIXME
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
