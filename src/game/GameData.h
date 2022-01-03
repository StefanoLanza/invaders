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
	spawnWave,
};

struct Event;

struct Level
{
	const Event* events;
	int numEvents;
};

struct AlienPrefab
{
	Animation anim;
	Color     color;
	int       health;
	const char* actionSeq;
	float hspeed;
	float vspeed;
	float fireRate;
};


struct PlayerPrefab
{
	ImageId imageId;
	Color   color;
	Color   invulnColor;
	float   velocity;
	float   laserOffset;
};


int GetNumLevels();
const Level& GetLevel(int index);
const AlienPrefab& GetAlienPrefab(int index);
const PlayerPrefab& GetPlayerPrefab(int index);
