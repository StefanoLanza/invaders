#include "stdafx.h"
#include "PowerUp.h"
#include <algorithm>
#include <cassert>


PowerUp::PowerUp(const Vector2D& initialPos, RaiderSprites sprite, float velocity, Type type) : 
	pos { initialPos },
	prevPos { initialPos },
	velocity { velocity },
	type { type },
	visual { initialPos, sprite, Color::yellow, nullptr },
	state { State::normal }
{}


const RenderItem& PowerUp::GetVisual() const
{
	return visual;
}


PowerUp::State PowerUp::GetState() const
{
	return state;
}


PowerUp::Type PowerUp::GetType() const
{
	return type;
}


void PowerUp::Destroy()
{
	state = State::dead;
}


CollisionArea PowerUp::GetCollisionArea() const
{
	return { prevPos, pos, 0.5f };
}


void PowerUp::Move(float dt, const Vector2D& worldBounds)
{
	prevPos = pos;
	// Move downwards
	pos.y += velocity * dt;
	if (pos.y > worldBounds.y - 0.5)
	{
		Destroy();
	}
	visual.pos = pos;
}
