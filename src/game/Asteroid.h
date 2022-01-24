#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>
#include <engine/Body.h>

struct Asteroid
{
	enum class State
	{
		falling,
		dead
	};

	Body       body;
	Visual     visual;
	AnimState  animState;
	State      state;
	int        enterDelay;
	int        hits;
};


struct Collider;

Asteroid NewAsteroid(const Vector2D& pos, const Vector2D& velocity, int enterDelay);
void UpdateAsteroid(Asteroid& asteroid, float dt, Vector2D worldBounds);
Collider GetCollider(Asteroid& asteroid);
void HitAsteroid(Asteroid& asteroid);
void DestroyAsteroid(Asteroid& asteroid);
