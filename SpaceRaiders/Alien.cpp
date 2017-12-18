#include "stdafx.h"
#include "Alien.h"
#include "Prefabs.h"
#include "PlayField.h"
#include "Laser.h"
#include "GameConfig.h"
#include "Collision.h"
#include "Images.h"
#include <random>
#include <algorithm>
#include <cassert>


namespace
{

using MoveScript = void (*)(Body& body, float dt, const Vector2D& worldBounds);
using AIScript = void (*)(Alien& alien, const Body& body, float dt, PlayField& world, const GameConfig& gameConfig);


void MoveReflect(Body& body, float dt, const Vector2D& worldBounds)
{
	const float halfWidth = body.size.x * 0.5f;
	body.prevPos = body.pos;
	body.pos = Mad(body.pos, body.velocity, dt);

	// Border check
	if (body.pos.x < halfWidth)
	{
		body.pos.x = halfWidth;
		body.pos.y += body.size.y;
		body.velocity.x = std::abs(body.velocity.x);
	}
	else if (body.pos.x > worldBounds.x - halfWidth)
	{
		body.pos.x = (float)worldBounds.x - halfWidth;
		body.velocity.x = -std::abs(body.velocity.x);
		body.pos.y += body.size.y;
	}
}

const MoveScript moveScripts[MoveScriptId::count] = 
{
	&MoveReflect
};


const AlienPrefab* GetActivePrefab(const Alien& alien)
{
	return alien.state == Alien::State::better ? alien.betterPrefab : alien.normalPrefab;
}


void AlienSetFireTimer(Alien& alien, const GameConfig& gameConfig)
{
	alien.gameState.fireTimer = alien.state == Alien::State::normal ? gameConfig.alienFireRate : gameConfig.betterAlienFireRate;
	// Randomize it
	alien.gameState.fireTimer *= (1.f + alien.rndFloat(*alien.rGen));
}


void AlienTransformIntoBetter(Alien& alien)
{
	alien.state = Alien::State::better;
	alien.animState.frame = 0;
	const float sign = alien.body.velocity.x > 0.f ? -1.f : +1.f;
	alien.body.velocity.x = alien.betterPrefab->speed * sign;
	alien.gameState.health = alien.betterPrefab->health;
}


void AlienScript(Alien& alien, const Body& body, float dt, PlayField& world, const GameConfig& gameConfig)
{
	AlienGameState& gameState = alien.gameState;
	const Vector2D size = body.size;
	// Border check vertical:
	if (body.pos.y >= world.GetBounds().y - size.y)
	{
		// If an alien ship reaches the bottom of the screen the players die
		world.DeletePlayers();
	}
	if (body.pos.y > world.GetBounds().y - size.y * 0.5f)
	{
		alien.state = Alien::State::dead;  // destroy the ship
	}

	// The amount of energy increases randomly per frame
	gameState.energy += alien.rndFloat(*alien.rGen) * gameConfig.alienUpdateRate;

	// State machine
	switch (alien.state)
	{
		case Alien::State::normal:
			if (gameState.energy >= gameConfig.alienTransformEnergy)
			{
				// The alien ship can transform into a better Alien. The actual transformation happens randomly.
				alien.state = Alien::State::evolving;
			}
			break;
		case Alien::State::evolving:
			if (alien.rndFloat(*alien.rGen) < gameConfig.alienTransformRate)
			{
				// Transform into a better alien
				AlienTransformIntoBetter(alien);
			}
			break;
		default:
			break;
	};

	if (gameState.fireTimer == 0.f)
	{ 
		AlienSetFireTimer(alien, gameConfig);
	}
	gameState.fireTimer -= dt;
	// Randomly shoot lasers
	if (gameState.fireTimer < 0.f)
	{
		const Vector2D laserPos = { body.pos.x, body.pos.y + size.y * 0.5f }; // spawn in front
		world.SpawnAlienLaser( { laserPos, { 0.f, gameConfig.alienLaserVelocity }, { ImageId::alienLaser, Color::greenIntense }, -1, ColliderId::alienLaser } );
		gameState.fireTimer = 0.f; // reset it
	}
}


const AIScript aiScripts[AIScriptId::count] = 
{
	&AlienScript
};


}


void Alien_Move(Alien& alien, float dt, const Vector2D& worldBounds)
{
	const Image& image = GetImage( GetActivePrefab(alien)->anim.images[alien.animState.frame] );
	alien.body.size = { (float)image.width, (float)image.height };
	moveScripts[(uint)alien.moveScriptId](alien.body, dt, worldBounds);
}


void Alien_Update(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	UpdateAnimation(alien.animState, alien.state == Alien::State::better ? alien.betterPrefab->anim : alien.normalPrefab->anim, dt);
	aiScripts[(uint)alien.aiScriptId](alien, alien.body, dt, world, gameConfig);

	const AlienPrefab* prefab = GetActivePrefab(alien);
	alien.visual.imageId = prefab->anim.images[alien.animState.frame];
	alien.visual.color =  prefab->color;
}


Alien Alien_New(const Vector2D& initialPos, const Vector2D& velocity, std::default_random_engine& rGen, const AlienPrefab& normalPrefab,
	const AlienPrefab& betterPrefab)
{
	Alien alien;
	alien.body = { initialPos, initialPos, velocity };
	alien.normalPrefab = &normalPrefab;
	alien.betterPrefab = &betterPrefab;
	alien.rndFloat.param( std::uniform_real_distribution<float>::param_type { 0.f, 1.f } );
	alien.rGen = &rGen;
	alien.moveScriptId = MoveScriptId::move;
	alien.aiScriptId = AIScriptId::alien;
	// Set default
	alien.gameState.health = normalPrefab.health;
	alien.gameState.fireTimer  = 0.f;
	alien.gameState.energy = 0.f;
	alien.state = Alien::State::normal;
	return alien;
}


RenderItem Alien_GetRenderItem(const Alien& alien)
{
	return { alien.body.pos, alien.visual };
}


Collider Alien_GetCollider(Alien& alien)
{
	return { &alien, ColliderId::alien, alien.body.prevPos, alien.body.pos, alien.body.size };
}


void Alien_AvoidWall(Alien& alien, const Vector2D& wallPos)
{
	Body& body = alien.body;
	const Vector2D size = alien.body.size;
	// Reflect trajectory against wall (quad shape)
	const Vector2D wallNormal = ComputeClosestNormal(body.velocity);
	Vector2D vr = Reflect(body.velocity, wallNormal);
	vr = Normalize(vr);
	Vector2D closestWallPoint = Mad(wallPos, wallNormal, 2.f); 
	Vector2D collisionPoint = closestWallPoint;
	// Push alien position a safe distance away from the wall so that it doesn't get stuck in a collision loop
	body.prevPos = body.pos;
	body.pos = Mad(collisionPoint, vr, size.x * 1.5f); // FIXME
	// Update velocity
	body.velocity = Mul(vr, Length(body.velocity));
	body.velocity.y = std::max(0.f, body.velocity.y); // don't go upwards
}


void Alien_DecreaseHealth(Alien& alien)
{ 
	alien.gameState.health -= 1; 
	if (alien.gameState.health <= 0)
	{
		alien.state = Alien::State::dead;
	}
}


void Alien_Destroy(Alien& alien)
{
	alien.state = Alien::State::dead;
}
