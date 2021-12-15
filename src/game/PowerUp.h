#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>


class PlayField;
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
		invulnerability,
		bomb,
		shield,
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
Collider GetCollisionArea(PowerUp& powerUp);
void Move(PowerUp& powerUp, float dt, const Vector2D& worldBounds);
void Destroy(PowerUp& powerUp);
