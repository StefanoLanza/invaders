#pragma once

#include "Image.h"

enum class ImageId
{
	null,
	planet,
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
	playerLaserLeft,
	playerLaserRight,
	alienLaser,
	player,
	explosion,
	dPowerUp,
	tPowerUp,
	sPowerUp,
	fPowerUp,
	iPowerUp,
	shPowerUp,
	shield,
	solidWall,
	brittleWall,
	bomb,
	snowFlake,
	gift,
	happyHolidays,
	xmasLeaf,
	level,
	girl,
	// numbers
	_0,
	_1,
	_2,
	_3
};
constexpr size_t numImages = static_cast<size_t>(ImageId::_3) + 1;

const Image& GetImage(ImageId imageId);

struct Vector2D;
Vector2D GetImageSize(ImageId imageId);