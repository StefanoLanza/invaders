#include "Asteroid.h"
#include "Images.h"
#include <engine/Collision.h>
#include <algorithm>
#include <cassert>


constexpr Animation asteroidAnim = 
{
	{ GameImageId::asteroid0, GameImageId::asteroid1 }, .12f
};


Asteroid NewAsteroid(const Vector2D& initialPos, int enterDelay)
{
	Asteroid a;
	a.body.pos = initialPos;
	a.body.prevPos = initialPos;
	a.body.size = GetImageSize(GameImageId::asteroid0);
	a.visual = { GameImageId::asteroid0, Color::yellowIntense };
	a.enterDelay = enterDelay;
	a.state = Asteroid::State::falling;
	return a;
}


void UpdateAsteroid(Asteroid& asteroid, float dt, Vector2D worldBounds)
{
	UpdateAnimation(asteroid.animState, asteroidAnim, dt);

	--asteroid.enterDelay;
	if (asteroid.enterDelay < 0)
	{
		asteroid.visual.imageId = asteroidAnim.images[asteroid.animState.frame];
		asteroid.body.size = GetImageSize(asteroid.visual.imageId);
		if (asteroid.state == Asteroid::State::falling)
		{
			asteroid.body.prevPos = asteroid.body.pos;
			constexpr float aspectRatio = 0.5f; // console chars are two times taller than wide
			Vector2D velocity = asteroid.body.velocity;
			velocity.y *= aspectRatio;
			asteroid.body.pos = Mad(asteroid.body.pos, velocity, dt);
		}
		if (asteroid.body.pos.y >= worldBounds.y)
		{
			asteroid.state = Asteroid::State::dead;
		}
	}
}


Collider GetCollisionArea(Asteroid& asteroid)
{
	return { &asteroid, ColliderId::player, asteroid.body.prevPos, asteroid.body.pos, asteroid.body.size };
}
