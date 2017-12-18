#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include "Body.h"
#include "Collision.h"


class Laser
{
public:

	enum class State
	{
		normal,
		dead
	};

	Laser(const Vector2D& initialPos, const Vector2D& velocity, const Visual& visual, int ownerId, ColliderId colliderId);

	const Visual& GetVisual() const;
	const Vector2D& GetPosition() const;
	Collider GetCollider();
	State GetState() const;
	int GetOwnerId() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Destroy();

private:

	Body   body;
	Visual visual;
	int    ownerId; // keep track of what player shot the laser, to update its score
	ColliderId colliderId;
	State  state;
};

