#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>

struct Explosion
{
	float      timer;
	float      delay;
	Vector2D   pos;
	Visual     visual;
	AnimState  animState;
};


Explosion NewExplosion(const Vector2D& initialPos, float timer, float delay);
void UpdateExplosion(Explosion& explosion, float dt);
bool IsExplosionOver(const Explosion& explosion);