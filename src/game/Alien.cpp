#include "Alien.h"
#include "Prefabs.h"
#include "PlayField.h"
#include "Laser.h"
#include "GameConfig.h"
#include <engine/Collision.h>
#include "Images.h"
#include <engine/ScriptModule.h>
#include <random>
#include <algorithm>
#include <cassert>


namespace
{

const AlienPrefab& GetActivePrefab(const Alien& alien)
{
	return *(alien.state == Alien::State::better ? alien.betterPrefab : alien.normalPrefab);
}

}


void UpdateAlien(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig, const ScriptModule& scriptModule)
{
	const Image& image = GetImage( GetActivePrefab(alien).anim.images[alien.animState.frame] );
	alien.body.size = { (float)image.width, (float)image.height };

	UpdateAnimation(alien.animState, alien.state == Alien::State::better ? alien.betterPrefab->anim : alien.normalPrefab->anim, dt);
//	RunAIScript(alien.aiScriptId, alien, scriptArgs);

	const AlienPrefab& prefab = GetActivePrefab(alien);
	alien.visual.imageId = prefab.anim.images[alien.animState.frame];
	alien.visual.color =  prefab.color;

	if (scriptModule.ai)
	{
		ScriptArgs scriptArgs = { dt, nullptr, &world, &gameConfig };
		scriptModule.ai(alien.scriptId, alien, scriptArgs);
	}
}


Alien NewAlien(const Vector2D& initialPos, const Vector2D& velocity, const AlienPrefab& normalPrefab, const AlienPrefab& betterPrefab)
{
	Alien alien;
	alien.body = { initialPos, initialPos, velocity, {0.f, 0.f} };
	alien.normalPrefab = &normalPrefab;
	alien.betterPrefab = &betterPrefab;
	alien.scriptId = AlienScriptId::alien;
	// Set default
	alien.gameState.health = normalPrefab.health;
	alien.gameState.fireTimer  = 0.f;
	alien.gameState.energy = 0.f;
	alien.state = Alien::State::normal;
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


void HitAlien(Alien& alien)
{ 
	alien.gameState.health -= 1; 
	if (alien.gameState.health <= 0)
	{
		alien.state = Alien::State::dead;
	}
}


void DestroyAlien(Alien& alien)
{
	alien.state = Alien::State::dead;
}
