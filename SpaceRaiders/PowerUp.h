#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


class PlayField;
struct Collider;


class PowerUp
{
public:

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

	PowerUp(const Vector2D& initialPos, const Visual& visual, const Vector2D& size, float velocity, Type type);

	const Vector2D& GetPosition() const;
	Collider GetCollisionArea();
	State GetState() const;
	Type GetType() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Destroy();
	const Visual& GetVisual() const;

private:

	Vector2D   pos;
	Vector2D   prevPos;
	float      velocity;
	Type       type;
	Visual     visual;
	Vector2D   size;
	State      state;
};
