#include "Alien.h"
#include "GameData.h"
#include "PlayField.h"
#include "GameConfig.h"
#include "Images.h"
#include <engine/Collision.h>
#include <algorithm>
#include <cassert>


Alien NewAlien(const Vector2D& initialPos, const Vector2D& velocity, const AlienPrefab& prefab, float randomOffset)
{
	Alien alien;
	alien.body = { initialPos, initialPos, velocity, {0.f, 0.f} };
	alien.prefab = &prefab;
	// Set default
	alien.gameState.health = prefab.health;
	alien.gameState.fireTimer  = 0.f;
	alien.gameState.energy = 0.f;
	alien.state = Alien::State::alive;
	alien.animState.t = randomOffset;
	alien.waveIndex = -1;
	alien.indexInWave = -1;
	alien.randomOffset = randomOffset;
	return alien;
}

void AlienUpdate(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	const Image& image = GetImage( alien.prefab->anim.images[alien.animState.frame] );
	alien.body.size = { (float)image.width, (float)image.height };
	UpdateAnimation(alien.animState, alien.prefab->anim, dt);
	alien.visual.imageId = alien.prefab->anim.images[alien.animState.frame];
	alien.visual.color =  alien.prefab->color;
}

RenderItem AlienGetRenderItem(const Alien& alien)
{
	return { alien.body.pos, alien.visual };
}


Collider AlienGetCollider(Alien& alien)
{
	return { &alien, ColliderId::alien, alien.body.prevPos, alien.body.pos, alien.body.size };
}


bool AlienHit(Alien& alien)
{ 
	alien.gameState.health -= 1; 
	return alien.gameState.health <= 0;
}


void AlienDestroy(Alien& alien, AlienWave& wave)
{
	if (alien.state != Alien::State::dead) 
	{
		alien.state = Alien::State::dead;
		assert(wave.numAliens > 0);
		--wave.numAliens;
		assert(wave.mask[alien.indexInWave] == 1);
		wave.mask[alien.indexInWave] = 0;
	}
}

void BossDestroy(Alien& alien)
{
	if (alien.state != Alien::State::dead)
	{
		alien.state = Alien::State::dead;
	}
}
