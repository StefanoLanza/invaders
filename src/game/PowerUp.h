#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>


struct Collider;


struct PowerUp
{
	enum class State
	{
		normal,
		dead
	};

	enum Type
	{
		speedBoost,
		fireBoost,
		doubleFire,
		tripleFire,
		shield,
		bomb,
		count
	};

	Vector2D   pos;
	Vector2D   prevPos;
	float      velocity;
	Type       type;
	Visual     visual;
	Vector2D   size;
	State      state;
};

PowerUp NewPowerUp(const Vector2D& initialPos, const Visual& visual, const Vector2D& size, float velocity, PowerUp::Type type);
Collider PowerUpGetCollider(PowerUp& powerUp);
void PowerUpMove(PowerUp& powerUp, float dt, const Vector2D& worldBounds);
void PowerUpDestroy(PowerUp& powerUp);
