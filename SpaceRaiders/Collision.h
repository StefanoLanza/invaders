#pragma once

#include "Base.h"
#include "Vector2D.h"


enum class CollisionId : uint8
{
	player,
	alien,
	playerLaser,
	alienLaser,
	powerUp,
	wall
};

struct Collider
{
	Vector2D    v0;
	Vector2D    v1;
	Vector2D    size;
	void*       userData;
	CollisionId id;
};


struct CollisionInfo
{
//	Vector2D point;
//	Vector2D normal;
	void*       ud0;
	void*       ud1;
	CollisionId id0;
	CollisionId id1;
};

struct Rectangle
{
	Vector2D v0;
	Vector2D v1;
};


Vector2D ComputeClosestNormal(const Vector2D& v);

bool Intersect(const Collider& a, const Collider& b);