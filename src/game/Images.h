#pragma once

#include <engine/Image.h>
#include <cstddef>

enum GameImageId : ImageId
{
	planet = 1,
	alien0_0,
	alien0_1,
	alien1_0,
	alien1_1,
	alien2_0,
	alien2_1,
	alien3_0,
	alien3_1,
	boss0_0,
	boss0_1,
	boss1_0,
	boss1_1,
	boss2_0,
	boss2_1,
	playerLaser,
	alienLaser,
	player,
	explosion0,
	explosion1,
	dPowerUp,
	tPowerUp,
	sPowerUp,
	fPowerUp,
	iPowerUp,
	shield,
	solidWall,
	brittleWall,
	bomb,
	snowFlake,
	gift,
	happyHolidays,
	xmasLeaf,
	stage,
	score,
	gameOver,
	// numbers
	_0,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	_9,
};

void InitGameImages();
