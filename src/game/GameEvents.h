#include <engine/Event.h>
#include "Images.h"
#include <engine/Colors.h>


struct AlienSquad 
{
	int w;
	int h;
	float dx;
	float dy;
	const char* squad;
};

struct AlienWaveInfo
{
	const AlienSquad* squad;
	float start_y;
	float initialDirection;
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
};

struct Level
{
	const Event* events;
	int numEvents;
};

int GetNumLevels();
const Level& GetLevel(int index);