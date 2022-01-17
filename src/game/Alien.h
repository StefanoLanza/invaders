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
	int          hits;
	float        energy;
	float        fireTimer;
	float        speed;
	Vector2D     gridPos;
};


struct ActionSeq 
{
	const char* seq;
	int ticks;
	int a;
	int length;
	int ticksPerAction;
	bool looping;
};

struct AlienWave {
	int numAliens;
	float speed;
	float fireRate;
};

struct Alien
{
	enum class State
	{
		landing,
		parking,
		attacking,
		dead
	};

	// Dependencies
	const AlienPrefab* prefab;

	Body      body;
	Visual    visual;
	AnimState animState;
	AlienGameState gameState;
	State   state;
	Vector2D prevVel;
	Vector2D nextVel;

	ActionSeq actionSeq;
	int waveIndex;
	float randomOffset; // [0,1]
};


// Public API
Alien NewAlien(const Vector2D& initialPos, const Vector2D& gridPos, const AlienPrefab& prefab, float randomOffset);
void AlienDestroy(Alien& alien);
RenderItem AlienGetRenderItem(const Alien& alien);
Collider AlienGetCollider(Alien& alien);
void AlienUpdate(Alien& alien, float dt, PlayField& world, const GameConfig& config);
bool AlienHit(Alien& alien);
