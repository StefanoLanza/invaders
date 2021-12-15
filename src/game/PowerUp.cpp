#include "PowerUp.h"
#include "Images.h"
#include <engine/Collision.h>
#include <algorithm>
#include <cassert>


PowerUp NewPowerUp(const Vector2D& initialPos, const Visual& visual, const Vector2D& size, float velocity, PowerUp::Type type)
{
	return { initialPos,
		initialPos,
		velocity,
		type,
		visual,
		size,
	PowerUp::State::normal };
}


void Destroy(PowerUp& powerUp)
{
	powerUp.state = PowerUp::State::dead;
}


Collider GetCollisionArea(PowerUp& powerUp)
{
	return { &powerUp, ColliderId::powerUp, powerUp.prevPos, powerUp.pos, powerUp.size, };
}


void Move(PowerUp& powerUp, float dt, const Vector2D& worldBounds)
{
	powerUp.prevPos = powerUp.pos;
	// Move downwards
	powerUp.pos.y += powerUp.velocity * dt;
	if (powerUp.pos.y > worldBounds.y - 0.5)
	{
		Destroy(powerUp);
	}
}
