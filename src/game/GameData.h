#include <engine/Colors.h>
#include <engine/RenderItem.h>
#include <scripts/Scripts.h>
#include "Images.h"

struct PathEntry
{
    int dir;
	int duration;
};

constexpr size_t maxPathLen = 50;
constexpr size_t maxAliensPerWave = 60;

struct Path
{
	float startx;
	float starty;
	const PathEntry entries[maxPathLen];
};

struct AlienWaveInfo
{
	const char* mask;
	const int enterDelay[maxAliensPerWave];
	const int enterPath[maxAliensPerWave];
	const int attackPath[maxAliensPerWave];
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
	float speed;
	float enterSpeed;
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
const Path& GetEnterPath(int index);
const Path& GetAttackPath(int index);
