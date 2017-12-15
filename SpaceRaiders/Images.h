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
	alien3,
	boss0_0,
	boss0_1,
	boss1,
	boss2,
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
	solidWall,
	brittleWall,
	bomb,
	count
};

const Image& GetImage(ImageId imageId);

struct Vector2D;
Vector2D GetImageSize(ImageId imageId);