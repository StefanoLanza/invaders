#pragma once

#include "Image.h"

enum class ImageId
{
	null,
	planet,
	alien0,
	alien1,
	alien2,
	alien3,
	boss0,
	boss1,
	boss2,
	boss3,
	playerLaser,
	playerLaserLeft,
	playerLaserRight,
	alienLaser,
	player,
	explosion,
	tPowerUp,
	sPowerUp,
	fPowerUp,
	iPowerUp,
	solidWall,
	brittleWall,
	count
};

const Image* GetImage(ImageId imageId);

struct Vector2D;
Vector2D GetImageSize(ImageId imageId);