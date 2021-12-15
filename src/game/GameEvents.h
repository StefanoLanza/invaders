#include <engine/Event.h>
#include "Images.h"
#include <engine/Colors.h>


struct AlienWave
{
	int   n;
	float y;
	float dx;
	float direction;
	int   alienType;
	int   betterAlienType;
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