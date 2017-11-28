#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include <random>


class PlayField;
struct GameConfig;


class Alien
{
public:

	enum class State
	{
		normal,
		evolving,
		better,
		dead
	};

	Alien(const Vector2D& initialPos, const Vector2D& velocity, float health, std::default_random_engine& rGen);

	const RenderItem& GetVisual() const;
	const Vector2D& GetPosition() const;
	CollisionArea GetCollisionArea() const;
	State GetState() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Update(float dt, PlayField& world, const GameConfig& config);
	void Destroy();
	void DecreaseHealth();
	void AvoidWall(const Vector2D& wallPos);

private:

	void SetFireTimer(const GameConfig& gameConfig);
	void Transform(const GameConfig& config);

private:

	State       state;
	Vector2D   pos;
	Vector2D   prevPos;
	RenderItem  visual;
	std::uniform_real_distribution<float> rndFloat;
	std::default_random_engine* rGen;
	Vector2D velocity;
	float health;
	float energy;
	float damageOnHit;
	float fireTimer;
};
