#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


class PlayField;


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
		tripleFire,
		invulnerability,
		count
	};

	PowerUp(const Vector2D& initialPos, RaiderSprites sprite, float velocity, Type type);

	CollisionArea GetCollisionArea() const;
	State GetState() const;
	Type GetType() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Destroy();
	const RenderItem& GetVisual() const;

private:

	Vector2D   pos;
	Vector2D   prevPos;
	float      velocity;
	Type       type;
	RenderItem visual;
	State      state;
};
