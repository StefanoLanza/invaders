#include <engine/Event.h>
#include <engine/Colors.h>
#include <engine/RenderItem.h>
#include <scripts/Scripts.h>
#include "Images.h"

struct AlienSquad 
{
	int numCols;
	int numRows;
	const char* squad;
};

struct AlienWaveInfo
{
	const AlienSquad* squad;
	float dx;
	float dy;
	float start_y;
	float initialDirection;
	float initialSpeed;
	float initialFireRate;
	float bound;
};


struct WallInfo
{
	int n;
	float x, y;
};

struct BossInfo
{
	float x, y;
	int   alienType;
	float speed;
};

using EventId = int;
struct Event;

enum EventType : EventId
{
	wait,
	message,
	showStage,
	hideStage,
	spawnWave,
	boss,
};

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
	AlienScriptId behaviour;
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
const AlienPrefab& GetBossPrefab(int index);
const PlayerPrefab& GetPlayerPrefab(int index);
