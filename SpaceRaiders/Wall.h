#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


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
	CollisionArea GetCollisionArea() const;
	State GetState() const;
	const RenderItem& GetVisual() const;
	void Hit();

private:

	Vector2D   pos;
	RenderItem visual;
	State      state;
	int        hits;
};
