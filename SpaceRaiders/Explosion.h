#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


struct Collider;


class Explosion
{
public:

	Explosion(const Vector2D& initialPos, float timer, float delay);

	const Vector2D& GetPosition() const;
	const Visual& GetVisual() const;
	bool Update(float dt);

private:

	float      timer;
	float      delay;
	Vector2D   pos;
	Visual     visual;
};
