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
	float        speed;
};


struct ActionSeq 
{
	const char* seq;
	int ticks;
	int l;
	int a;
};

struct AlienWave {
	int numAliens;
	int numCols;
	int numRows;
	float speed;
	float fireRate;
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

	ActionSeq actionSeq;
	int waveIndex;
	int indexInWave;
	float randomOffset; // [0,1]
};


struct AIModule;

// Public API
Alien NewAlien(const Vector2D& initialPos, const AlienPrefab& prefab, float randomOffset);
void AlienDestroy(Alien& alien, AlienWave& wave);
RenderItem AlienGetRenderItem(const Alien& alien);
Collider AlienGetCollider(Alien& alien);
void AlienUpdate(Alien& alien, float dt, PlayField& world, const GameConfig& config);
bool AlienHit(Alien& alien);
