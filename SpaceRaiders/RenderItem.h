#pragma once

#include "Vector2D.h"
#include "Colors.h"


enum class RaiderSprites : unsigned char
{
	backgroundTile = 0xB0,
	player1 = '1',
	player2 = '2',
	alien = 'A',
	betterAlien = 'B',
	playerLaser_0 = 0xBA,  // straight
	playerLaser_1 = '\\',  // left
	playerLaser_2 = '/',   // right
	alienLaser = '|',
	explosion = '*',
	powerUp_0 = 'S',
	powerUp_1 = 'F',
	powerUp_2 = 'T',
	powerUp_3 = 'I',
	wallSolid = 177,
	wallBrittle = 176
};


struct Image;

struct RenderItem
{
	Vector2D      pos;
	RaiderSprites sprite;
	Color         color;
	const Image*  image;
};
