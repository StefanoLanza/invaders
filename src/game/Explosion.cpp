#include "Explosion.h"
#include "Images.h"
#include <algorithm>
#include <cassert>


Explosion NewExplosion(const Vector2D& initialPos, float timer, float delay)
{
	Explosion e;
	e.pos = initialPos;
	e.visual = { GetImageId(GameImageId::explosion), Color::yellowIntense };
	e.timer = timer;
	e.delay = delay;
	return e;
}


bool UpdateExplosion(Explosion& explosion, float dt)
{
	assert(explosion.timer > 0.f);
	if (explosion.delay > dt)
	{
		explosion.delay -= dt;
		explosion.visual.imageId = ImageId::null;
	}
	else
	{ 
		explosion.delay = 0.f;
		explosion.visual.imageId = GetImageId(GameImageId::explosion);
		explosion.timer -= dt;
	}
	// Delete if timer <= 0.f
	return (explosion.timer <= 0.f);
}
