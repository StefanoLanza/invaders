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

	// Scripts
	AlienScriptId scriptId;
};


struct ScriptModule;

// Public API
Alien NewAlien(const Vector2D& initialPos, const Vector2D& velocity, const AlienPrefab& normalPrefab, 
		const AlienPrefab& betterPrefab);
void DestroyAlien(Alien& alien);
RenderItem GetRenderItem(const Alien& alien);
Collider GetCollider(Alien& alien);
void UpdateAlien(Alien& alien, float dt, PlayField& world, const GameConfig& config, const ScriptModule& scriptModule);
void HitAlien(Alien& alien);
void Alien_AvoidWall(Alien& alien, const Vector2D& wallPos);
