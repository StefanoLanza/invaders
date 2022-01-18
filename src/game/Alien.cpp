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

/*
   0
12    4
   8
*/
constexpr int moves[16][2] = {
    {0,-4}, // 0, top
    {1,-4},
    {3,-3},
    {4,-1},
    {4,0}, // 4, right
    {4,1},
    {3,3},
    {1,4},
    {0,4},  // 8, bottom
    {-1,4},
    {-3,3},
    {-4,1},
    {-4,0}, // 12, left
    {-4,-1},
    {-3,-3},
    {-1,-4}
};

void InitActionSequence(ActionSeq& seq, const Path& path, bool looping)
{
	int pathLen = 0;
	while (path.entries[pathLen].dir >= 0) ++pathLen;
	seq.a = (pathLen > 0) ? 0 : -1;
	seq.duration = 1;
	seq.seq = path.entries;
	seq.length = pathLen;
	seq.looping = looping;
}


void FollowPath(Alien& alien, const PathEntry& entry)
{
	const float speed = alien.gameState.speed;
	float xs = (1.f / 400) * 160.f;
	float ys = (1.f / 500) * 54.f;
	xs *= 60.f;
	ys = xs * 0.5f; //60.f;
	Vector2D velocity { (float)moves[entry.dir][0] * xs, (float)moves[entry.dir][1] * ys };
	//velocity = Normalize(velocity, speed);
	//velocity = Mul(velocity, speed);
	alien.body.velocity = velocity;
}

bool TickAlien(Alien& alien, ActionSeq& seq) 
{
	if (seq.a <= -1)
	{
		return false;
	}

	--seq.duration;
	if (seq.duration == 0)
	{
		seq.duration = seq.seq[seq.a].duration; //FIXME
		FollowPath(alien, seq.seq[seq.a]);
		++seq.a;
		if (seq.a >= seq.length)
		{
			seq.a = seq.looping ? 0 : -1;
		}
	}

	return true;
}


}


Alien NewAlien(const Vector2D& gridPos, const AlienPrefab& prefab, float randomOffset, int enterDelay, const Path& enterPath)
{
	Vector2D initialPos { enterPath.startx, enterPath.starty };
	Alien alien;
	alien.body = { initialPos, initialPos, { 0.f, 0.f }, {0.f, 0.f} };
	alien.enterDelay = enterDelay;
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
	InitActionSequence(alien.actionSeq, enterPath, false);
	return alien;
}


void ParkAlien(Alien& alien)
{
	Vector2D diff = Sub(alien.gameState.gridPos, alien.body.pos);
	float sd = SquareLength(diff); 
	if (sd < 0.25f)
	{
		alien.body.velocity = { 0.f , 0.f};
		alien.gameState.speed = alien.prefab->speed;
		alien.state = Alien::State::attacking;
		//FIXME InitActionSequence(alien.actionSeq, alien.prefab->attackSeq, true);
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
			--alien.enterDelay;
			if (alien.enterDelay < 0 && ! TickAlien(alien, alien.actionSeq))
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
