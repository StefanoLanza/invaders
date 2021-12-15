#include "Laser.h"
#include "Images.h"
#include <algorithm>
#include <cassert>



Laser NewLaser(const Vector2D& initialPos, const Vector2D& velocity, const Visual& visual, int ownerId, ColliderId colliderId)
{
	Laser laser;
	laser.body = { initialPos, initialPos, velocity, { 2.f, 2.f } };
	laser.visual = visual;
	laser.ownerId = ownerId;
	laser.colliderId = colliderId;
	laser.state = Laser::State::normal;
	return laser;
}


Collider GetCollider(Laser& laser)
{
	return { &laser, laser.colliderId, laser.body.prevPos, laser.body.pos, laser.body.size };
}


void MoveLaser(Laser& laser, float dt, const Vector2D& worldBounds)
{
	Body& body = laser.body;
	body.prevPos = body.pos;
	body.pos = Mad(body.pos, body.velocity, dt);
	// Note: not specified in the rules, I choose to destroy the laser if it moves outside the world bounds in any direction
	if (body.pos.y < 0.5 || body.pos.y > worldBounds.y - 0.5f || body.pos.x < 0.5 || body.pos.x > worldBounds.x - 0.5f)
	{
		DestroyLaser(laser);
	}
}

void DestroyLaser(Laser& laser)
{
	laser.state = Laser::State::dead;
}
