#include "Alien.h"
#include "Prefabs.h"
#include "PlayField.h"
#include "Laser.h"
#include "GameConfig.h"
#include <engine/Collision.h>
#include "Images.h"
#include "AIModule.h"
#include <random>
#include <algorithm>
#include <cassert>


void UpdateAlien(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	const Image& image = GetImage( alien.prefab->anim.images[alien.animState.frame] );
	alien.body.size = { (float)image.width, (float)image.height };

	UpdateAnimation(alien.animState, alien.prefab->anim, dt);

	alien.visual.imageId = alien.prefab->anim.images[alien.animState.frame];
	alien.visual.color =  alien.prefab->color;
}


Alien NewAlien(const Vector2D& initialPos, const Vector2D& velocity, const AlienPrefab& prefab)
{
	Alien alien;
	alien.body = { initialPos, initialPos, velocity, {0.f, 0.f} };
	alien.prefab = &prefab;
	alien.scriptId = AlienScriptId::alien;
	// Set default
	alien.gameState.health = prefab.health;
	alien.gameState.fireTimer  = 0.f;
	alien.gameState.energy = 0.f;
	alien.state = Alien::State::alive;
	alien.waveIndex = -1;
	alien.indexInWave = -1;
	return alien;
}


RenderItem GetRenderItem(const Alien& alien)
{
	return { alien.body.pos, alien.visual };
}


Collider GetCollider(Alien& alien)
{
	return { &alien, ColliderId::alien, alien.body.prevPos, alien.body.pos, alien.body.size };
}


bool HitAlien(Alien& alien)
{ 
	alien.gameState.health -= 1; 
	return alien.gameState.health <= 0;
}


void DestroyAlien(Alien& alien, AlienWave& wave)
{
	if (alien.state != Alien::State::dead) {
		alien.state = Alien::State::dead;
		assert(wave.numAliens > 0);
		--wave.numAliens;
		assert(wave.mask[alien.indexInWave] == 1);
		wave.mask[alien.indexInWave] = 0;
	}
}
