#include <engine/Colors.h>
#include <engine/RenderItem.h>
#include <scripts/Scripts.h>
#include "Images.h"

struct AlienWaveInfo
{
	const char* mask;
	int numCols;
	int numRows;
	float dx;
	float dy;
	float start_y;
};


struct WallInfo
{
	int n;
	float x, y;
};

using EventId = int;

enum GameEventId : EventId
{
	wait,
	message,
	showStage,
	hideStage,
	showScore,
	hideScore,
	spawnWave,
};

struct Event;

struct Stage
{
	const Event* events;
	int numEvents;
};

struct Plan;

//using ActionSeq = const char*;

struct AlienPrefab
{
	Animation anim;
	Color     color;
	int       hits;
	const char* landingSeq;
	const char* attackSeq;
	float speed;
	float landingSpeed;
	float fireRate;
	float laserSpeed;
	bool  aimAtPlayer;
};


struct PlayerPrefab
{
	ImageId imageId;
	float   velocity;
	float   laserOffset;
};


int GetNumStages();
const Stage& GetStage(int index);
const AlienPrefab& GetAlienPrefab(int index);
const PlayerPrefab& GetPlayerPrefab(int index);
