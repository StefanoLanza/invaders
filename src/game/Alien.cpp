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


namespace
{

const AlienPrefab& GetActivePrefab(const Alien& alien)
{
	return *(alien.state == Alien::State::better ? alien.betterPrefab : alien.normalPrefab);
}

}


void UpdateAlien(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig, const AIModule& aiModule)
{
	const Image& image = GetImage( GetActivePrefab(alien).anim.images[alien.animState.frame] );
	alien.body.size = { (float)image.width, (float)image.height };

	UpdateAnimation(alien.animState, alien.state == Alien::State::better ? alien.betterPrefab->anim : alien.normalPrefab->anim, dt);

	const AlienPrefab& prefab = GetActivePrefab(alien);
	alien.visual.imageId = prefab.anim.images[alien.animState.frame];
	alien.visual.color =  prefab.color;

	if (aiModule.procedure)
	{
		ScriptArgs scriptArgs = { dt, nullptr, &world, &gameConfig };
		aiModule.procedure(alien.scriptId, alien, scriptArgs);
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
	alien.state = Alien::State::dead;
	assert(wave.numAliens > 0);
	--wave.numAliens;
	wave.mask[alien.indexInWave] = 0;
}
