#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


struct Collider;


struct Wall
{
	enum class State
	{
		normal,
		dead
	};

	Vector2D   pos;
	Visual     visual;
	State      state;
	int        hits;
};


Wall NewWall(const Vector2D& pos, int maxHits);
Collider GetCollisionArea(Wall& wall);
void HitWall(Wall& wall);
