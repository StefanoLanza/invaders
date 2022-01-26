#pragma once

#include <engine/Vector2D.h>
#include <engine/Colors.h>

using ImageId = int;

struct Particle
{
	Vector2D pos;
	Vector2D vel;
	Vector2D accel;
	int      life;
	Color    color;
	ImageId  imageId;
};
