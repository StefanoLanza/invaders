#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


class Explosion
{
public:

	Explosion(const Vector2D& initialPos, float timer);

	const Vector2D& GetPosition() const;
	const RenderItem& GetVisual() const;
	bool Update(float dt);

private:

	
	float      timer;
	Vector2D   pos;
	RenderItem visual;
};
