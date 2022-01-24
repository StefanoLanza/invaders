#pragma once

#include <engine/Vector2D.h>
#include <engine/Colors.h>

using ImageId = int;
enum GameImageId : ImageId;

struct Particle
{
	Vector2D pos;
	Vector2D vel;
	Vector2D accel;
	int      life;
	Color    color;
	GameImageId imageId;
};
