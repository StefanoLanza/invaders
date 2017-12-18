#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include "Body.h"
#include "Collision.h"
#include <random>


class PlayField;
struct GameConfig;
struct AlienPrefab;
struct Collider;
struct Image;


enum class MoveScriptId
{
	move = 0,
	count
};

enum class AIScriptId
{
	alien = 0,
	count
};


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
		evolving,
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
	std::default_random_engine* rGen;
	const AlienPrefab* normalPrefab;
	const AlienPrefab* betterPrefab;
	std::uniform_real_distribution<float> rndFloat;

	// Scripts
	MoveScriptId moveScriptId;
	AIScriptId   aiScriptId;
};


// Public API
Alien Alien_New(const Vector2D& initialPos, const Vector2D& velocity, std::default_random_engine& rGen, const AlienPrefab& normalPrefab, 
		const AlienPrefab& betterPrefab);
void Alien_Destroy(Alien& alien);
RenderItem Alien_GetRenderItem(const Alien& alien);
Collider Alien_GetCollider(Alien& alien);
void Alien_Move(Alien& alien, float dt, const Vector2D& worldBounds);
void Alien_Update(Alien& alien, float dt, PlayField& world, const GameConfig& config);
void Alien_DecreaseHealth(Alien& alien);
void Alien_AvoidWall(Alien& alien, const Vector2D& wallPos);
