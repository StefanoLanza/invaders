#include "stdafx.h"
#include "PowerUp.h"
#include "Images.h"
#include "Collision.h"
#include <algorithm>
#include <cassert>


PowerUp::PowerUp(const Vector2D& initialPos, const Visual& visual, const Vector2D& size, float velocity, Type type) : 
	pos { initialPos },
	prevPos { initialPos },
	velocity { velocity },
	type { type },
	visual { visual },
	size { size },
	state { State::normal }
{}


const Vector2D& PowerUp::GetPosition() const
{
	return pos;
}


const Visual& PowerUp::GetVisual() const
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


Collider PowerUp::GetCollisionArea()
{
	return { prevPos, pos, size, this, CollisionId::powerUp };
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
}
