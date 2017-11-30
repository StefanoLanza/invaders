// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Event.h"
#include "Images.h"
#include "Colors.h"


struct AlienWave
{
	int   n;
	float y;
	float dx;
	float direction;
	bool  split;
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