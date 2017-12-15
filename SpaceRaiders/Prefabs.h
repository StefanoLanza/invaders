// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "RenderItem.h"


struct AlienPrefab
{
	Animation anim;
	Color     color;
	int       health;
	float     speed;
};


const AlienPrefab& GetAlienPrefab(int index);
const AlienPrefab& GetBossPrefab(int index);


struct PlayerPrefab
{
	ImageId imageId;
	Color   color;
	Color   invulnColor;
	float   velocity;
	float   laserOffset;
};

const PlayerPrefab& GetPlayerPrefab(int index);
