#include "stdafx.h"
#include "Laser.h"
#include "Images.h"
#include "Collision.h"
#include <algorithm>
#include <cassert>



Laser::Laser(const Vector2D& initialPos, const Vector2D& velocity, const Visual& visual, int ownerId, ColliderId colliderId) :
	body{ initialPos, initialPos, velocity, { 1.f, 1.f } },
	visual { visual },
	ownerId { ownerId },
	colliderId { colliderId },
	state { State::normal }
{}


const Visual& Laser::GetVisual() const
{
	return visual;
}


const Vector2D& Laser::GetPosition() const
{
	return body.pos;
}


Collider Laser::GetCollider()
{
	return { this, colliderId, body.prevPos, body.pos, body.size };
}


Laser::State Laser::GetState() const
{
	return state;
}


int Laser::GetOwnerId() const
{
	return ownerId;
}


void Laser::Move(float dt, const Vector2D& worldBounds)
{
	body.prevPos = body.pos;
	body.pos = Mad(body.pos, body.velocity, dt);
	// Note: not specified in the rules, I choose to destroy the laser if it moves outside the world bounds in any direction
	if (body.pos.y < 0.5 || body.pos.y > worldBounds.y - 0.5f || body.pos.x < 0.5 || body.pos.x > worldBounds.x - 0.5f)
	{
		Destroy();
	}
}

void Laser::Destroy()
{
	state = State::dead;
}
