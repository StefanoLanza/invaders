#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include "Image.h"


class PlayField;


class AlienLaser
{
public:

	enum class State
	{
		normal,
		dead
	};

	AlienLaser(const Vector2D& initialPos, float velocity);

	const Vector2D& GetPosition() const;
	State GetState() const;
	CollisionArea GetCollisionArea() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Destroy();
	const RenderItem& GetVisual() const;

private:

	Vector2D   pos;
	Vector2D   prevPos;
	float      velocity;
	RenderItem visual;
	State      state;
};
