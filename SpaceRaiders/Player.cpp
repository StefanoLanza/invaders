#include "stdafx.h"
#include "Player.h"
#include "PlayField.h"
#include "PlayerLaser.h"
#include "Input.h"
#include <random>
#include <algorithm>
#include <cassert>


PlayerShip::PlayerShip(const Vector2D& initialPos,  float velocity, int id, std::shared_ptr<Input> input_, std::default_random_engine& rGen) : 
	pos { initialPos },
	prevPos { initialPos },
	visual { initialPos, id == 0 ? RaiderSprites::player1 : RaiderSprites::player2, Color::white, nullptr },
	id { id },
	velocity { velocity },
	fireTimer { 0.f },
	fireBoost { 1.f },
	state { State::normal },
	speedBoost { 1.f },
	score { 0 },
	tripleFire { false },
	input { std::move(input_) },
	rGen(&rGen),
	rndFloat01 { 0, 1, },
	boostTimer { 0.f },
	invulnerabilityTime { 0.f },
	accumTime { 0.f }
{
}


void PlayerShip::Move(float dt, const Vector2D& worldBounds)
{
	// Update input state
	input->Update(dt);

	prevPos = pos;
	// The original code did not handle the first and last column robustly
	if (input->Left())
	{
		pos.x -= velocity * speedBoost * dt;
		pos.x = std::max(0.5f, pos.x);
	}
	else if (input->Right())
	{
		pos.x += velocity * speedBoost * dt;
		pos.x = std::min(pos.x, worldBounds.x - 0.5f);
	}
	visual.pos = pos;

	// Boosts lasts a certain amount of time
	boostTimer -= dt;
	if (boostTimer < 0.f)
	{
		// Restore normal values
		boostTimer = 0.f;
		speedBoost = 1.f;
		fireBoost = 1.f;
	}

	accumTime += dt; // useful for time based effects

	invulnerabilityTime = std::max(0.f, invulnerabilityTime - dt);
	if (invulnerabilityTime > 0)
	{
		// Flicker player color to indicate invulnerability
		visual.color = std::sin(20.f * accumTime) > 0.f ? Color::yellow : Color::white;
	}
	else
	{
		visual.color = Color::white;
	}
}


void PlayerShip::ShootLasers(float dt, PlayField& world, float laserVelocity, float fireRate)
{
	if (fireTimer == 0.f)
	{
		fireTimer = fireRate * (1.f + rndFloat01(*rGen)) * fireBoost;
	}
	fireTimer -= dt;

	// Randomly shoot laser shots
	if (fireTimer < 0.f)
	{
		fireTimer = 0.f; // reset it
		const float l = laserVelocity;
		const Vector2D laserPos = { pos.x, pos.y - 1.f }; // spawn in front
		if (tripleFire)
		{
			if (world.GetAvailablePlayerLasers() >= 3)
			{
				world.SpawnPlayerLaser( PlayerLaser { laserPos, {  0, -l }, RaiderSprites::playerLaser_0, id } );  // straight
				world.SpawnPlayerLaser( PlayerLaser { laserPos, { -l, -l }, RaiderSprites::playerLaser_1, id } );  // 45 left
				world.SpawnPlayerLaser( PlayerLaser { laserPos, { +l, -l }, RaiderSprites::playerLaser_2, id } );  // 45 right
			}
			else
			{
				// Try shooting later
			}
		}
		else
		{
			world.SpawnPlayerLaser( PlayerLaser { laserPos, { 0, -l }, RaiderSprites::playerLaser_0, id } );  // straight
		}
	}
}


void PlayerShip::Destroy()
{
	if (invulnerabilityTime == 0.f)
	{
		state = State::dead;
	}
}


const Vector2D& PlayerShip::GetPosition() const
{
	return pos;
}


CollisionArea PlayerShip::GetCollisionArea() const
{
	return { prevPos, pos, 0.5f };
}


const RenderItem& PlayerShip::GetVisual() const
{
	return visual;
}


PlayerShip::State PlayerShip::GetState() const
{
	return state;
}


void PlayerShip::SetSpeedBoost(float value)
{
	assert(value > 0);
	speedBoost = value;
	boostTimer = 10.f;
}


void PlayerShip::SetFireBoost(float boost)
{
	assert(boost > 0.f);
	fireBoost = 1.f / boost; // it decrease interval between shots
	boostTimer = 10.f; // Note specified in the rules, I'm giving the fire boost a fixed amount of time too
}


void PlayerShip::SetTripleFire(bool enabled)
{
	tripleFire = enabled;
}


void PlayerShip::SetInvulnerable(float timer)
{
	invulnerabilityTime = timer;
}


void PlayerShip::AddScore(int increment)
{
	assert(increment > 0);
	score += increment;
}


int PlayerShip::GetScore() const 
{
	return score; 
}


int PlayerShip::GetId() const
{
	return id;
}
