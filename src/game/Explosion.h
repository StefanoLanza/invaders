#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>

struct Explosion
{
	float      timer;
	float      delay;
	Vector2D   pos;
	Visual     visual;
};


Explosion NewExplosion(const Vector2D& initialPos, float timer, float delay);
bool UpdateExplosion(Explosion& explosion, float dt);
