// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Event.h"


struct AlienWave
{
	int   n;
	float y;
	float dx;
	float direction;
	bool  split;
};


struct WallInfo
{
	int n;
	float x, y;
};

struct BossInfo
{
	float x, y;
};


extern const Event gameEvents[];
extern const int numGameEvents;
extern const char* hudMessages[];
extern const int numHUDMessages;
extern const AlienWave alienWaves[];
extern const WallInfo walls[];
extern const BossInfo bossInfo[];
