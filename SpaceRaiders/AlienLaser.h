#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include "Image.h"


class PlayField;
struct Collider;


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
	Collider GetCollisionArea();
	void Move(float dt, const Vector2D& worldBounds);
	void Destroy();
	const Visual& GetVisual() const;

private:

	Vector2D   pos;
	Vector2D   prevPos;
	float      velocity;
	Visual     visual;
	State      state;
};
