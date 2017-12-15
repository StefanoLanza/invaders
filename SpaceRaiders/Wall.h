#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


struct Collider;


class Wall
{
public:

	enum class State
	{
		normal,
		dead
	};

	Wall(const Vector2D& pos, int maxHits);

	const Vector2D& GetPosition() const;
	Collider GetCollisionArea();
	State GetState() const;
	const Visual& GetVisual() const;
	void Hit();

private:

	Vector2D   pos;
	Visual     visual;
	State      state;
	int        hits;
};
