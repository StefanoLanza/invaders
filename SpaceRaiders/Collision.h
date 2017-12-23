#pragma once

#include "Base.h"
#include "Vector2D.h"


enum class ColliderId : uint8
{
	player,
	alien,
	playerLaser,
	alienLaser,
	powerUp,
	wall,
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


struct CollisionInfo
{
//	Vector2D point;
//	Vector2D normal;
	void*       ud0;
	void*       ud1;
	ColliderId id0;
	ColliderId id1;
};

struct Rectangle
{
	Vector2D v0;
	Vector2D v1;
};


Vector2D ComputeClosestNormal(const Vector2D& v);

bool Intersect(const Rectangle& a, const Rectangle& b);
