#include "stdafx.h"
#include "Explosion.h"
#include <algorithm>
#include <cassert>


Explosion::Explosion(const Vector2D& initialPos, float timer) :
	pos { initialPos },
	visual { initialPos, RaiderSprites::explosion, Color::yellowIntense, nullptr },
	timer { timer }
{}


const Vector2D& Explosion::GetPosition() const
{
	return pos;
}


const RenderItem& Explosion::GetVisual() const
{
	return visual;
}


bool Explosion::Update(float dt)
{
	assert(timer > 0.f);
	timer -= dt;
	// Delete if timer <= 0.f
	return (timer <= 0.f);
}
