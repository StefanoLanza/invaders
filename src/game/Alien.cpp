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

bool TickAlien(Alien& alien, ActionSeq& seq) 
{
	const int dticks = seq.ticksPerAction;
	if (seq.a >= seq.length && seq.ticks == dticks - 1) {
		return false;
	}

	if (seq.ticks == 0  && seq.length > 0) {
		ProcessAction(alien, seq.seq[seq.a]);
		++seq.a;
		if (seq.a >= seq.length && seq.looping) {
			seq.a = 0;
		}
	}
	const float t = (float)seq.ticks / (float)dticks;

	// Next tick
	seq.ticks++;
	if (seq.ticks >= dticks) {
		seq.ticks = 0;
	}

	alien.body.velocity = Lerp(alien.prevVel, alien.nextVel, t);
	return true;
}


}


Alien NewAlien(const Vector2D& initialPos, const Vector2D& gridPos, const AlienPrefab& prefab, float randomOffset)
{
	Alien alien;
	alien.body = { initialPos, initialPos, { 0.f, 0.f }, {0.f, 0.f} };
	alien.prevVel = alien.nextVel = { 0,0};
	alien.prefab = &prefab;
	alien.visual.imageId = nullImageId;
	alien.visual.color = Color::white;
	// Set default
	alien.gameState.hits = prefab.hits;
	alien.gameState.fireTimer  = 0.f;
	alien.gameState.energy = 0.f;
	alien.gameState.speed = prefab.landingSpeed;
	alien.gameState.gridPos = gridPos;
	alien.state = Alien::State::landing;
	alien.animState.t = randomOffset;
	alien.waveIndex = -1;
	alien.randomOffset = randomOffset;
	InitActionSequence(alien.actionSeq, prefab.landingSeq, false);
	return alien;
}


void ParkAlien(Alien& alien)
{
	Vector2D diff = Sub(alien.gameState.gridPos, alien.body.pos);
	float sd = SquareLength(diff); 
	if (sd < 0.5f)
	{
		alien.body.velocity = { 0.f , 0.f};
		alien.gameState.speed = alien.prefab->speed;
		alien.state = Alien::State::attacking;
		InitActionSequence(alien.actionSeq, alien.prefab->attackSeq, true);
	}
	else
	{
		float speed = alien.prefab->landingSpeed * std::clamp(sd / 16.f, 0.f, 1.f);
		alien.body.velocity = Normalize(diff, speed);
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
			if (! TickAlien(alien, alien.actionSeq))
			{
				alien.state = Alien::State::parking;
			}
			break;
		case Alien::State::parking:
			ParkAlien(alien);
			break;
		case Alien::State::attacking:
			TickAlien(alien, alien.actionSeq);
			break;
		case Alien::State::dead:
			// Wait
			break;
	}

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


void AlienDestroy(Alien& alien)
{
	if (alien.state != Alien::State::dead) 
	{
		alien.state = Alien::State::dead;
	}
}
