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
	int numCols;
	int numRows;
	float direction;
	float speed;
	float fireRate;
	float bound;
	uint8_t mask[4 * 8]; // FIXME optimize
};

struct Alien
{
	enum class State
	{
		alive,
		dead
	};

	// Dependencies
	const AlienPrefab* prefab;

	// Components
	Body      body;
	Visual    visual;
	AnimState animState;
	AlienGameState gameState;
	State   state;

	int waveIndex;
	int indexInWave;
	float randomOffset; // [0,1]

	// Scripts
	AlienScriptId scriptId;
};


struct AIModule;

// Public API
Alien NewAlien(const Vector2D& initialPos, const Vector2D& velocity, const AlienPrefab& prefab, float randomOffset);
void AlienDestroy(Alien& alien, AlienWave& wave);
RenderItem AlienGetRenderItem(const Alien& alien);
Collider AlienGetCollider(Alien& alien);
void AlienUpdate(Alien& alien, float dt, PlayField& world, const GameConfig& config);
bool AlienHit(Alien& alien);
