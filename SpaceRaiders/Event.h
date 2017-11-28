// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


enum class EventType
{
	randomMessage,
	spawnWave,
	spawnWalls,
	boss
};


struct Event
{
	EventType type;
	float     time;
	int       data;
};
