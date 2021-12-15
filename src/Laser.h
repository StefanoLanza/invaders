#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include "Body.h"
#include "Collision.h"


struct Laser
{
	enum class State
	{
		normal,
		dead
	};

	Body   body;
	Visual visual;
	int    ownerId; // keep track of what player shot the laser, to update its score
	ColliderId colliderId;
	State  state;
};

Laser NewLaser(const Vector2D& initialPos, const Vector2D& velocity, const Visual& visual, int ownerId, ColliderId colliderId);
void MoveLaser(Laser& laser, float dt, const Vector2D& worldBounds);
void DestroyLaser(Laser& laser);
Collider GetCollider(Laser& laser);
