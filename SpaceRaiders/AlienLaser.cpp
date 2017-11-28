#include "stdafx.h"
#include "AlienLaser.h"
#include "Images.h"
#include <algorithm>
#include <cassert>


AlienLaser::AlienLaser(const Vector2D& initialPos, float velocity) : 
	pos { initialPos },
	prevPos { initialPos },
	velocity { velocity },
	visual { initialPos, RaiderSprites::alienLaser, Color::greenIntense, &laserImage },
	state { State::normal }
{}


const RenderItem& AlienLaser::GetVisual() const
{
	return visual;
}


const Vector2D& AlienLaser::GetPosition() const
{
	return pos;
}

CollisionArea AlienLaser::GetCollisionArea() const
{
	return { prevPos, pos, 0.25f };
}


AlienLaser::State AlienLaser::GetState() const
{
	return state;
}


void AlienLaser::Destroy()
{
	state = State::dead;
}


void AlienLaser::Move(float dt, const Vector2D& worldBounds)
{
	prevPos = pos;
	pos.y += velocity * dt;
	if (pos.y > worldBounds.y - 0.5f)
	{
		Destroy();
	}
	visual.pos = pos;
}
