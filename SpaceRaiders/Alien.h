#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include "Body.h"
#include "Collision.h"
#include "src/Scripts.h"
#include <random>


class PlayField;
struct GameConfig;
struct AlienPrefab;
struct Collider;


struct AlienGameState
{
	int          health;
	float        energy;
	float        fireTimer;
};


struct Alien
{
	enum class State
	{
		normal,
		better,
		dead
	};

	// Components
	Body      body;
	Visual    visual;
	AnimState animState;
	AlienGameState gameState;
	State   state;

	// Dependencies
	const AlienPrefab* normalPrefab;
	const AlienPrefab* betterPrefab;

	// Scripts
	AlienScriptId scriptId;
};


struct ScriptModule;

// Public API
Alien NewAlien(const Vector2D& initialPos, const Vector2D& velocity, const AlienPrefab& normalPrefab, 
		const AlienPrefab& betterPrefab);
void DestroyAlien(Alien& alien);
RenderItem Alien_GetRenderItem(const Alien& alien);
Collider Alien_GetCollider(Alien& alien);
void Alien_Update(Alien& alien, float dt, PlayField& world, const GameConfig& config, const ScriptModule& scriptModule);
void Alien_DecreaseHealth(Alien& alien);
void Alien_AvoidWall(Alien& alien, const Vector2D& wallPos);
