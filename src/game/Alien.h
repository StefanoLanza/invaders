#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>
#include <engine/Body.h>
#include <engine/Collision.h>
#include <scripts/Scripts.h>
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


struct AlienWave {
	int numAliens;
	float direction;
	uint8_t mask[4 * 8]; // FIXME optimize
};

struct Alien
{
	enum class State
	{
		normal,
		better,
		dead
	};

	// Dependencies
	const AlienPrefab* normalPrefab;
	const AlienPrefab* betterPrefab;

	// Components
	Body      body;
	Visual    visual;
	AnimState animState;
	AlienGameState gameState;
	State   state;

	int waveIndex;
	int indexInWave;

	// Scripts
	AlienScriptId scriptId;
};


struct AIModule;

// Public API
Alien NewAlien(const Vector2D& initialPos, const Vector2D& velocity, const AlienPrefab& normalPrefab, 
		const AlienPrefab& betterPrefab);
void DestroyAlien(Alien& alien, AlienWave& wave);
RenderItem GetRenderItem(const Alien& alien);
Collider GetCollider(Alien& alien);
void UpdateAlien(Alien& alien, float dt, PlayField& world, const GameConfig& config, const AIModule& aiModule);
bool HitAlien(Alien& alien);
