#include "stdafx.h"
#include "Alien.h"
#include "PlayField.h"
#include "AlienLaser.h"
#include "GameConfig.h"
#include "Images.h"
#include <random>
#include <algorithm>
#include <cassert>


Alien::Alien(const Vector2D& initialPos, const Vector2D& velocity, float health, std::default_random_engine& rGen) :
	pos { initialPos },
	prevPos { initialPos },
	visual { initialPos, RaiderSprites::alien, Color::redIntense, &alien0Img },
	rndFloat { 0, 1 },
	rGen { &rGen },
	health { health },
	velocity { velocity },
	fireTimer { 0.f },
	state { State::normal }
{
	// Set default
	damageOnHit = 1.f;
	energy = 0.f;
}


const RenderItem& Alien::GetVisual() const
{
	return visual;
}


const Vector2D& Alien::GetPosition() const
{
	return pos;
}


CollisionArea Alien::GetCollisionArea() const
{
	return { prevPos, pos, 0.5f };
}


Alien::State Alien::GetState() const
{
	return state;
}


void Alien::Move(float dt, const Vector2D& worldBounds)
{
	prevPos = pos;
	pos = Mad(pos, velocity, dt);

	// Border check
	if (pos.x < 0.5f)
	{
		pos.x = 0.5f;
		pos.y += 1.f;
		velocity.x = std::abs(velocity.x);
	}
	else if (pos.x > (float)worldBounds.x - 0.5f)
	{
		pos.x = (float)worldBounds.x - 0.5f;
		velocity.x = -std::abs(velocity.x);
		pos.y += 1.f;
	}

	visual.pos = pos;
}


void Alien::Update(float dt, PlayField& world, const GameConfig& gameConfig)
{
	// Border check vertical:
	if (pos.y >= world.GetBounds().y - 1)
	{
		// If an alien ship reaches the bottom of the screen the players die
		world.EndGame();
	}
	if (pos.y > world.GetBounds().y - 0.5f)
	{
		Destroy();  // destroy the ship
	}

	// The amount of energy increases randomly per frame
	energy += rndFloat(*rGen) * gameConfig.alienUpdateRate;

	// State machine
	switch (state)
	{
		case State::normal:
			if (energy >= gameConfig.alienTransformEnergy)
			{
				// The alien ship can transform into a better Alien. The actual transformation happens randomly.
				state = State::evolving;
			}
			break;
		case State::evolving:
			if (rndFloat(*rGen) < gameConfig.alienTransformRate)
			{
				// Transform into a better alien
				Transform(gameConfig);
			}
			break;
		default:
			break;
	};

	if (fireTimer == 0.f)
	{ 
		SetFireTimer(gameConfig);
	}
	fireTimer -= dt;
	// Randomly shoot lasers
	if (fireTimer < 0.f)
	{
		const Vector2D laserPos = { pos.x, pos.y + 1.f }; // spawn in front
		world.SpawnAlienLaser( AlienLaser { laserPos, gameConfig.alienLaserVelocity } );
		fireTimer = 0.f; // reset it
	}
}


void Alien::Destroy()
{
	state = State::dead;
}


void Alien::DecreaseHealth() 
{ 
	health -= damageOnHit; 
	if (health <= 0.f)
	{
		state = State::dead;
	}
}


void Alien::AvoidWall(const Vector2D& wallPos)
{
	// Reflect trajectory against wall (quad shape)
	const Vector2D wallNormal = ComputeClosestNormal(velocity);
	Vector2D vr = Reflect(velocity, wallNormal);
	vr = Normalize(vr);
	Vector2D closestWallPoint = Mad(wallPos, wallNormal, 0.5f); 
	Vector2D collisionPoint = closestWallPoint;
	// Push alien position a safe distance away from the wall so that it doesn't get stuck in a collision loop
	prevPos = pos;
	pos = Mad(collisionPoint, vr, 0.75f); 
	visual.pos = pos;
	// Update velocity
	velocity = Mul(vr, Length(velocity));
	velocity.y = std::max(0.f, velocity.y); // don't go upwards
}


void Alien::Transform(const GameConfig& gameConfig)
{
	state = State::better;
	visual.image = &alien1Img;
	velocity.x *= gameConfig.betterAlienAcceleration;
	damageOnHit = 0.5f; // it takes two laser shots to destroy a better alien ship.
}


void Alien::SetFireTimer(const GameConfig& gameConfig)
{
	fireTimer = state == State::normal ? gameConfig.alienFireRate : gameConfig.betterAlienFireRate;
	// Randomize it
	fireTimer *= (1.f + rndFloat(*rGen));
}