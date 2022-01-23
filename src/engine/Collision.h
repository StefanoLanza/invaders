#pragma once

#include "Base.h"
#include "Vector2D.h"


// FIXME Not in engine
enum class ColliderId : uint8
{
	player,
	alien,
	playerLaser,
	alienLaser,
	powerUp,
	wall,
	asteroid,
	count
};


struct ColliderData
{
	void*      userData;
	ColliderId id;
};

struct Collider
{
	ColliderData data;
	Vector2D     v0;
	Vector2D     v1;
	Vector2D     size;
};

struct Rectangle
{
	Vector2D v0;
	Vector2D v1;
};


Vector2D ComputeClosestNormal(const Vector2D& v);

bool Intersect(const Rectangle& a, const Rectangle& b);
