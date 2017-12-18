#pragma once

#include "Vector2D.h"



struct Transform
{
	Vector2D pos;
	Vector2D prevPos;
	Vector2D velocity;
	Vector2D size;
};
