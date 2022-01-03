#include "Alien.h"
#include "GameData.h"
#include "PlayField.h"
#include "GameConfig.h"
#include "Images.h"
#include <engine/Collision.h>
#include <algorithm>
#include <cassert>


void ProcessAction(Alien& alien, const AlienWave& wave, char action) 
{
	const float speed = alien.gameState.speed; // * wave.speed;
	const float vspeed = alien.prefab->vspeed;
	switch (action) 
	{
		case 'l':
			alien.body.velocity = { -speed, 0.f };
			break;
		case 'r':
			alien.body.velocity = { speed, 0.f };
			break;
		case 't':
			alien.body.velocity = { 0.f, -vspeed };
			break;
		case 'b':
			alien.body.velocity = { 0.f, vspeed };
			break;
		default:
			break;
	}
}

void TickAlien(Alien& alien, const AlienWave& wave) 
{
	ActionSeq& actionSeq = alien.actionSeq;
	if (actionSeq.ticks == 0 && actionSeq.l > 0) {
		ProcessAction(alien, wave, actionSeq.seq[actionSeq.a]);
		actionSeq.a = (actionSeq.a + 1) % actionSeq.l;
	}
	constexpr int dticks = 30;
	actionSeq.ticks++;
	if (actionSeq.ticks >= dticks) {
		actionSeq.ticks = 0;
	}
//	float t = (float)actionSeq.ticks / (float)dticks;
}


Alien NewAlien(const Vector2D& initialPos, const AlienPrefab& prefab, float randomOffset)
{
	Alien alien;
	alien.body = { initialPos, initialPos, { 0.f, 0.f }, {0.f, 0.f} };
	alien.prefab = &prefab;
	// Set default
	alien.gameState.health = prefab.health;
	alien.gameState.fireTimer  = 0.f;
	alien.gameState.energy = 0.f;
	alien.gameState.speed = prefab.hspeed;
	alien.state = Alien::State::alive;
	alien.animState.t = randomOffset;
	alien.waveIndex = -1;
	alien.indexInWave = -1;
	alien.randomOffset = randomOffset;
	alien.actionSeq.a = 0;
	alien.actionSeq.ticks = 0;
	alien.actionSeq.l = (int)strlen(prefab.actionSeq);
	alien.actionSeq.seq = prefab.actionSeq;
	return alien;
}

void AlienUpdate(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	AlienWave& wave = world.alienWaves[alien.waveIndex];
	const Image& image = GetImage( alien.prefab->anim.images[alien.animState.frame] );
	alien.body.size = { (float)image.width, (float)image.height };
	UpdateAnimation(alien.animState, alien.prefab->anim, dt);
	alien.visual.imageId = alien.prefab->anim.images[alien.animState.frame];
	alien.visual.color =  alien.prefab->color;
	TickAlien(alien, wave);
	alien.body.prevPos = alien.body.pos;
	alien.body.pos = Mad(alien.body.pos, alien.body.velocity, dt);
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
