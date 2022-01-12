#pragma once

#include <engine/Vector2D.h>

struct Particle
{
	Vector2D pos;
	Vector2D vel;
	Vector2D accel;
	int      life;
};
