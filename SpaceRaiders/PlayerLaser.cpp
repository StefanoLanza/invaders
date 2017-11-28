#include "stdafx.h"
#include "PlayerLaser.h"
#include <algorithm>
#include <cassert>



PlayerLaser::PlayerLaser(const Vector2D& initialPos, const Vector2D& velocity, RaiderSprites sprite, int playerId) : 
	pos { initialPos },
	prevPos { initialPos },
	velocity { velocity },
	playerId { playerId },
	visual { initialPos, sprite, Color::white, nullptr },
	state { State::normal }
{}


const RenderItem& PlayerLaser::GetVisual() const
{
	return visual;
}


const Vector2D& PlayerLaser::GetPosition() const
{
	return pos;
}


CollisionArea PlayerLaser::GetCollisionArea() const
{
	return { prevPos, pos, 0.25f };
}


PlayerLaser::State PlayerLaser::GetState() const
{
	return state;
}


int PlayerLaser::GetPlayerId() const
{
	return playerId;
}


void PlayerLaser::Move(float dt, const Vector2D& worldBounds)
{
	prevPos = pos;
	pos = Mad(pos, velocity, dt);
	// Note: not specified in the rules, I choose to destroy the laser if it moves outside the world bounds in any direction
	if (pos.y < 0.5 || pos.y > worldBounds.y - 0.5f || pos.x < 0.5 || pos.x > worldBounds.x - 0.5f)
	{
		Destroy();
	}
	visual.pos = pos;
}


void PlayerLaser::Destroy()
{
	state = State::dead;
}
