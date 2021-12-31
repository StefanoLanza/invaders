#pragma once

#include <engine/Vector2D.h>
#include <engine/RenderItem.h>
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
		dead
	};

	void Destroy();
	void SetSpeedBoost(float boost);
	void SetFireBoost(float boost);
	void SetDoubleFire();
	void SetTripleFire();
	void SetInvulnerable(float timer);
	void AddScore(int increment);

	Vector2D   pos;
	const PlayerPrefab* prefab;
	Vector2D   prevPos;
	Visual     visual;
	Vector2D   size;
	int        id;
	float      fireTimer;
	int        score;
	State      state;
	float      accumTime;
	int        laserShots;
	// Power ups
	float      fireBoost;
	float      speedBoost;
	bool       doubleFire;
	bool       tripleFire;
	float      powerUpTimer;
	float      invulnerabilityTime;
	bool       hasShield;
	// Dependencies
	std::shared_ptr<Input> input;
};


PlayerShip NewPlayerShip(const Vector2D& initialPos, const PlayerPrefab& prefab, int id, std::shared_ptr<Input> input);
Collider GetCollisionArea(PlayerShip& ship);
void Move(PlayerShip& ship, float dt, const Vector2D& worldBounds, PlayField& world, const GameConfig& gameConfig);
