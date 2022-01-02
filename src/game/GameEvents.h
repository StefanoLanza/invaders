#include <engine/Event.h>
#include "Images.h"
#include <engine/Colors.h>


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

struct Level
{
	const Event* events;
	int numEvents;
};

int GetNumLevels();
const Level& GetLevel(int index);