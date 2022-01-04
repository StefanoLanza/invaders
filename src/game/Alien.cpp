#include "Alien.h"
#include "GameData.h"
#include "PlayField.h"
#include "GameConfig.h"
#include "Images.h"
#include <engine/Collision.h>
#include <algorithm>
#include <cassert>

namespace
{

void InitActionSequence(ActionSeq& seq, const char* str)
{
	seq.a = 0;
	seq.ticks = 0;
	seq.seq = str;
	seq.l = (int)strlen(str);
	seq.dir = 1;
}


void AlienSetVelocity(Alien& alien, const Vector2D& newVel) 
{
	alien.prevVel = alien.nextVel;
	alien.nextVel = newVel;
}

void ProcessAction(Alien& alien, const AlienWave& wave, char action, ActionSeq& seq) 
{
	const float speed = alien.gameState.speed; // * wave.speed;
	const float vspeed = alien.prefab->vspeed;
	switch (action) 
	{
		case 'l':
			AlienSetVelocity(alien, { -speed, 0.f });
			break;
		case 'r':
			AlienSetVelocity(alien, { speed, 0.f });
			break;
		case 't':
			AlienSetVelocity(alien, { 0.f, -vspeed });
			break;
		case 'b':
			AlienSetVelocity(alien, { 0.f, vspeed });
			break;
		case 's':
			AlienSetVelocity(alien, { 0.f, 0.f });
			break;
		case 'R':
			seq.dir = -seq.dir;
			break;
		default:
			break;
	}
}

void TickAlien(Alien& alien, const AlienWave& wave) 
{
	ActionSeq& seq = alien.actionSeq;
	if (seq.ticks == 0  && seq.l > 0) {
		ProcessAction(alien, wave, seq.seq[seq.a], seq);
		seq.a = (seq.a + seq.dir);
		if (seq.a >= seq.l) {
			seq.a = 0;
		}
		else if (seq.a < 0) 
		{
			seq.a = seq.l - 1;
		}
	}
	constexpr int dticks = 30;
	const float t = (float)seq.ticks / (float)dticks;

	// Next tick
	seq.ticks++;
	if (seq.ticks >= dticks) {
		seq.ticks = 0;
	}

	alien.body.velocity = Lerp(alien.prevVel, alien.nextVel, t);
}


}


Alien NewAlien(const Vector2D& initialPos, const AlienPrefab& prefab, float randomOffset)
{
	Alien alien;
	alien.body = { initialPos, initialPos, { 0.f, 0.f }, {0.f, 0.f} };
	alien.prevVel = alien.nextVel = { 0,0};
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
	InitActionSequence(alien.actionSeq, prefab.actionSeq);
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
