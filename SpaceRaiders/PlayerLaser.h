#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


class PlayerLaser
{
public:

	enum class State
	{
		normal,
		dead
	};

	PlayerLaser(const Vector2D& initialPos, const Vector2D& velocity, RaiderSprites sprite, int playerId);

	const RenderItem& GetVisual() const;
	const Vector2D& GetPosition() const;
	CollisionArea GetCollisionArea() const;
	State GetState() const;
	int GetPlayerId() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Destroy();

private:

	Vector2D   pos;
	Vector2D   prevPos;
	Vector2D   velocity;
	int        playerId; // keep track of what player shot the laser, to update its score
	RenderItem visual;
	State      state;
};

