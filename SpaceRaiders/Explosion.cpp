#include "Explosion.h"
#include "Images.h"
#include "Collision.h"
#include <algorithm>
#include <cassert>


Explosion NewExplosion(const Vector2D& initialPos, float timer, float delay)
{
	Explosion e;
	e.pos = initialPos;
	e.visual = { ImageId::explosion, Color::yellowIntense };
	e.timer = timer;
	e.delay = delay;
	return e;
}


bool Explosion::Update(float dt)
{
	assert(timer > 0.f);
	if (delay > dt)
	{
		delay -= dt;
		visual.imageId = ImageId::null;
	}
	else
	{ 
		delay = 0.f;
		visual.imageId = ImageId::explosion;
		timer -= dt;
	}
	// Delete if timer <= 0.f
	return (timer <= 0.f);
}
