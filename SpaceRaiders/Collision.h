#pragma once

#include "Vector2D.h"


enum class CollisionShape
{
	circle,
	rectangle,
	line
};

struct CollisionArea
{
	Vector2D v0;
	Vector2D v1;
	Vector2D size;
};

struct CollisionInfo
{
	Vector2D point;
	Vector2D normal;
	bool     valid;
};

struct Rectangle
{
	Vector2D v0;
	Vector2D v1;
};


Vector2D ComputeClosestNormal(const Vector2D& v);

bool Intersect(const CollisionArea& a, const CollisionArea& b);