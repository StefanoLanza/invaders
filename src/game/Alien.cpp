#include "Alien.h"
#include "GameData.h"
#include "PlayField.h"
#include "GameConfig.h"
#include "Images.h"
#include <engine/Collision.h>
#include <algorithm>
#include <cassert>
#include <cstring>

namespace
{

void InitActionSequence(ActionSeq& seq, const char* str, bool looping)
{
	seq.a = 0;
	seq.ticks = 0;
	seq.seq = str;
	seq.length = (int)strlen(str);
	seq.ticksPerAction = 30; // FIXME config
	seq.looping = looping;
	seq.dir = 1;
}


void AlienSetVelocity(Alien& alien, const Vector2D& newVel) 
{
	alien.prevVel = alien.nextVel;
	alien.nextVel = newVel;
}

void ProcessAction(Alien& alien, char action)
{
	const float speed = alien.gameState.speed;
	switch (action) 
	{
		case 'l':
			AlienSetVelocity(alien, { -speed, 0.f });
			break;
		case 'r':
			AlienSetVelocity(alien, { speed, 0.f });
			break;
		case 't':
			AlienSetVelocity(alien, { 0.f, -speed });
			break;
		case 'b':
			AlienSetVelocity(alien, { 0.f, speed });
			break;
		case 's':
			AlienSetVelocity(alien, { 0.f, 0.f });
			break;
		default:
			break;
	}
}

void TickAlien(Alien& alien, ActionSeq& seq) 
{
	const int dticks = seq.ticksPerAction;
	if (seq.a >= seq.length) {
		return;
	}

	if (seq.ticks == 0  && seq.length > 0) {
		ProcessAction(alien, seq.seq[seq.a]);
		seq.a = (seq.a + seq.dir);
		if (seq.a >= seq.length) {
			seq.a = 0;
		}
		else if (seq.a < 0) 
		{
			seq.a = seq.length - 1;
		}
	}
	const float t = (float)seq.ticks / (float)dticks;

	// Next tick
	seq.ticks++;
	if (seq.ticks >= dticks && seq.looping) {
		seq.ticks = 0;
	}

	alien.body.velocity = Lerp(alien.prevVel, alien.nextVel, t);
}


}


Alien NewAlien(const Vector2D& initialPos, const Vector2D& gridPos, const AlienPrefab& prefab, float randomOffset)
{
	Alien alien;
	alien.body = { initialPos, initialPos, { 0.f, 0.f }, {0.f, 0.f} };
	alien.prevVel = alien.nextVel = { 0,0};
	alien.prefab = &prefab;
	// Set default
	alien.gameState.hits = prefab.hits;
	alien.gameState.fireTimer  = 0.f;
	alien.gameState.energy = 0.f;
	alien.gameState.speed = prefab.speed;
	alien.gameState.gridPos = gridPos;
	alien.state = Alien::State::landing;
	alien.animState.t = randomOffset;
	alien.waveIndex = -1;
	alien.indexInWave = -1;
	alien.randomOffset = randomOffset;
//	InitActionSequence(alien.landingSeq, prefab.landingSeq, false);
//	InitActionSequence(alien.attackSeq, prefab.actionSeq, true);
	if (prefab.actionPlan)
	{
		InitPlan(alien.planState, *prefab.actionPlan);
	}
	return alien;
}


void ParkAlien(Alien& alien)
{
	Vector2D diff = Sub(alien.gameState.gridPos, alien.body.pos);
	if (SquareLength(diff) < 0.5f)
	{
		alien.body.velocity = { 0.f , 0.f};
		alien.state = Alien::State::ready;
	}
	else
	{
		alien.body.velocity = Normalize(diff, alien.gameState.speed);
	}
}


void AlienUpdate(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	const Image& image = GetImage( alien.prefab->anim.images[alien.animState.frame] );
	alien.body.size = { (float)image.width, (float)image.height };
	UpdateAnimation(alien.animState, alien.prefab->anim, dt);
	alien.visual.imageId = alien.prefab->anim.images[alien.animState.frame];
	alien.visual.color =  alien.prefab->color;

	switch (alien.state)
	{
		case Alien::State::landing:
			alien.state = Alien::State::parking; // TODO
			break;
		case Alien::State::parking:
			ParkAlien(alien);
			break;
		case Alien::State::ready:
			// Wait
			break;
		case Alien::State::attacking:
			if (alien.prefab->actionPlan)
			{
				TickPlan(alien.planState, *alien.prefab->actionPlan, alien.body.velocity, alien.body.pos);
			}
			break;
		case Alien::State::dead:
			// Wait
			break;
	}

/*	
	else {
		if (alien.state == Alien::State::landing)
		{
			TickAlien(alien, alien.landingSeq);
			if (alien.landingSeq.a >= alien.landingSeq.length)
			{
				alien.state = Alien::State::attacking;
			}
		}
		else if (alien.state == Alien::State::attacking)
		{
			TickAlien(alien, alien.attackSeq);
		}
	}
*/
	alien.body.prevPos = alien.body.pos;
	constexpr float aspectRatio = 0.5f; // console chars are two times taller than wide
	Vector2D velocity = alien.body.velocity;
	velocity.y *= aspectRatio;
	alien.body.pos = Mad(alien.body.pos, velocity, dt);
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
	alien.gameState.hits -= 1; 
	return alien.gameState.hits <= 0;
}


void AlienDestroy(Alien& alien, AlienWave& wave)
{
	if (alien.state != Alien::State::dead) 
	{
		alien.state = Alien::State::dead;
		assert(wave.numAliens > 0);
		--wave.numAliens;
	}
}
