#include "Explosion.h"
#include "Images.h"
#include <algorithm>
#include <cassert>


const Animation explosionAnim = 
{
	{ GameImageId::explosion0, GameImageId::explosion1 }, .12f
};


Explosion NewExplosion(const Vector2D& initialPos, float timer, float delay)
{
	Explosion e;
	e.pos = initialPos;
	e.visual = { GameImageId::explosion0, Color::yellowIntense };
	e.timer = timer;
	e.delay = delay;
	return e;
}


void UpdateExplosion(Explosion& explosion, float dt)
{
	assert(explosion.timer > 0.f);
	UpdateAnimation(explosion.animState, explosionAnim, dt);
	if (explosion.delay > dt)
	{
		explosion.delay -= dt;
		explosion.visual.imageId = nullImageId;
	}
	else
	{ 
		explosion.delay = 0.f;
		explosion.visual.imageId = explosionAnim.images[explosion.animState.frame];
		explosion.timer -= dt;
	}
}

bool IsExplosionOver(const Explosion& explosion) 
{
	// Delete if timer <= 0.f
	return (explosion.timer <= 0.f);
}
