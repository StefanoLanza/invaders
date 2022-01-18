#include <engine/Colors.h>
#include <engine/RenderItem.h>
#include <scripts/Scripts.h>
#include "Images.h"

struct PathEntry
{
    int dir;
	int duration;
};

struct Path
{
	float startx;
	float starty;
	const PathEntry* entries;
	int len;
};

struct AlienWaveInfo
{
	const char* mask;
	const int enterDelay[10*6];
	const int path[10*6];
	int numCols;
	int numRows;
	float dx;
	float dy;
	float start_y;
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

struct AlienPrefab
{
	Animation anim;
	Color     color;
	int       hits;
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
int GetNumAlienPrefabs();
const PlayerPrefab& GetPlayerPrefab(int index);
const Path& GetPath(int index);