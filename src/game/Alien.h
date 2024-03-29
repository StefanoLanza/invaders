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


struct PathEntry;
struct Path;

struct ActionSeq 
{
	const PathEntry* seq;
	int a;
	int length;
	int duration;
	bool looping;
};

struct AlienWave {
	int numAliens;
	int speedRate; // 1..100
	float fireRate;
	int numReadyAliens;
	int numAliveAliens;
};

struct Alien
{
	enum class State
	{
		entering,
		parking,
		ready,
		attacking,
		dying,
		dead
	};

	// Dependencies
	const AlienPrefab* prefab;

	Body      body;
	Visual    visual;
	AnimState animState;
	AlienGameState gameState;
	State   state;
	int enterDelay;
	const Path* attackPath;

	ActionSeq actionSeq;
	int waveIndex;
	float randomOffset; // [0,1]
};


// Public API
Alien NewAlien(const Vector2D& gridPos, const AlienPrefab& prefab, float randomOffset, 
	const Path& enterPath, int enterDelay,  
	const Path& attackPath);
void AlienDestroy(Alien& alien);
Collider AlienGetCollider(Alien& alien);
void AlienUpdate(Alien& alien, float dt, PlayField& world, const GameConfig& config, AlienWave& wave);
bool AlienHit(Alien& alien);
