#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>
#include <engine/Body.h>
#include <random>
#include <memory>


class PlayField;
class Input;
struct GameConfig;
struct PlayerPrefab;
struct Collider;


struct PlayerShip
{
	enum class State
	{
		normal,
		justHit,
		dead
	};

	void SetSpeedBoost(float boost);
	void SetFireBoost(float boost);
	void SetDoubleFire();
	void SetTripleFire();
	void SetInvulnerable(float timer);
	void SetShield(float time);
	void AddScore(int increment);

	Body      body;
	const PlayerPrefab* prefab;
	Visual     visual;
	int        id;
	float      fireTimer;
	int        score;
	State      state;
	float      accumTime;
	int        laserShots;
	int        lives;
	float      invulnerabilityTime;
	bool       visible;
	// Power ups
	float      fireBoost;
	float      speedBoost;
	bool       doubleFire;
	bool       tripleFire;
	float      powerUpTimer;
	float      shieldTime;
	bool       hasShield;
	Color      shieldColor;
	// Dependencies
	std::shared_ptr<Input> input;
};


PlayerShip NewPlayerShip(const Vector2D& initialPos, const PlayerPrefab& prefab, int id, std::shared_ptr<Input> input);
void PlayerHit(PlayerShip& player);
void KillPlayer(PlayerShip& player);
Collider GetCollisionArea(PlayerShip& ship);
void Move(PlayerShip& ship, float dt, const Vector2D& worldBounds, PlayField& world, const GameConfig& gameConfig);
