#include "Player.h"
#include "PlayField.h"
#include "Laser.h"
#include <engine/Input.h>
#include "Images.h"
#include "Prefabs.h"
#include "GameConfig.h"
#include <engine/Collision.h>
#include <random>
#include <algorithm>
#include <cassert>

void ShootLasers(PlayerShip& ship, float dt, PlayField& world, float laserVelocity, float fireRate)
{
	if (ship.fireTimer == 0.f)
	{
		ship.fireTimer = fireRate * ship.fireBoost;
	}
	ship.fireTimer -= dt;

	constexpr Visual laserVisual[3] =
	{
		{ GameImageId::playerLaser, Color::lightBlueIntense },
		{ GameImageId::playerLaserLeft, Color::lightBlueIntense },
		{ GameImageId::playerLaserRight, Color::lightBlueIntense },
	};

	// Randomly shoot laser shots
	if (ship.fireTimer < 0.f)
	{
		ship.fireTimer = 0.f; // reset it
		const float l = laserVelocity;
		// Spawn lasers in front
		const Vector2D laserPos_l = { ship.pos.x - ship.prefab->laserOffset, ship.pos.y - ship.size.y * 0.5f };
		const Vector2D laserPos_r = { ship.pos.x + ship.prefab->laserOffset, ship.pos.y - ship.size.y * 0.5f };
		const Vector2D laserPos_m = { ship.pos.x, ship.pos.y - ship.size.y * 0.5f };
		if (ship.doubleFire)
		{
			if (world.GetAvailablePlayerLasers() >= 2)
			{
				world.SpawnPlayerLaser( NewLaser(laserPos_l, { 0, -l }, laserVisual[0], ship.id, ColliderId::playerLaser) );
				world.SpawnPlayerLaser( NewLaser(laserPos_r, { 0, -l }, laserVisual[0], ship.id, ColliderId::playerLaser) );
			}
			else
			{
				// Try shooting later
			}
		}
		else if (ship.tripleFire)
		{
			if (world.GetAvailablePlayerLasers() >= 3)
			{
				world.SpawnPlayerLaser( NewLaser(laserPos_l, { 0, -l }, laserVisual[0], ship.id, ColliderId::playerLaser) );
				world.SpawnPlayerLaser( NewLaser(laserPos_r, { 0, -l }, laserVisual[0], ship.id, ColliderId::playerLaser) );
				world.SpawnPlayerLaser( NewLaser(laserPos_m, { 0, -l }, laserVisual[0], ship.id, ColliderId::playerLaser) );
			}
			else
			{
				// Try shooting later
			}
		}
		else // single fire
		{
			world.SpawnPlayerLaser( NewLaser(laserPos_m, { 0, -l }, laserVisual[0], ship.id, ColliderId::playerLaser) );  // straight
		}
		++ship.laserShots;
	}
}


PlayerShip NewPlayerShip(const Vector2D& initialPos, const PlayerPrefab& prefab, int id, std::shared_ptr<Input> input_)
{
	PlayerShip playerShip;
	playerShip.pos = initialPos;
	playerShip.prefab = &prefab;
	playerShip.prevPos = initialPos;
	playerShip.id = id;
	playerShip.fireTimer = 0.f;
	playerShip.fireBoost = 1.f;
	playerShip.state = PlayerShip::State::normal;
	playerShip.speedBoost = 1.f;
	playerShip.score = 0;
	playerShip.input = std::move(input_);
	playerShip.powerUpTimer = 0.f;
	playerShip.invulnerabilityTime = 0.f;
	playerShip.accumTime = 0.f;
	playerShip.size = GetImageSize(prefab.imageId);
	playerShip.laserShots = 0;
	playerShip.doubleFire = false;
	playerShip.tripleFire = false;
	playerShip.hasShield = false;
	return playerShip;
}


void Move(PlayerShip& ship, float dt, const Vector2D& worldBounds, PlayField& world, const GameConfig& gameConfig)
{
	// Update input state
	ship.input->Update(dt);

	const float halfWidth = ship.size.x / 2.f;

	ship.prevPos = ship.pos;
	// The original code did not handle the first and last column robustly
	if (ship.input->Left())
	{
		ship.pos.x -= ship.prefab->velocity * ship.speedBoost * dt;
		ship.pos.x = std::max(halfWidth, ship.pos.x);
	}
	else if (ship.input->Right())
	{
		ship.pos.x += ship.prefab->velocity * ship.speedBoost * dt;
		ship.pos.x = std::min(ship.pos.x, worldBounds.x - halfWidth);
	}
	if (ship.input->Fire()) {
		ShootLasers(ship, dt, world, gameConfig.playerLaserVelocity, gameConfig.playerFireRate);
	}

	// Boosts lasts a certain amount of time
	ship.powerUpTimer -= dt;
	if (ship.powerUpTimer < 0.f)
	{
		// Restore normal values
		ship.powerUpTimer = 0.f;
		ship.speedBoost = 1.f;
		ship.fireBoost = 1.f;
		ship.doubleFire = false;
		ship.tripleFire = false;
	}

	ship.accumTime += dt; // useful for time based effects

	ship.invulnerabilityTime = std::max(0.f, ship.invulnerabilityTime - dt);
	if (ship.invulnerabilityTime > 0)
	{
		// Flicker player color to indicate invulnerability
		ship.visual.color = std::sin(20.f * ship.accumTime) > 0.f ? ship.prefab->invulnColor : ship.prefab->color;
		ship.hasShield = true;
	}
	else
	{
		ship.visual.color = ship.prefab->color;
		ship.hasShield = false;
	}
	ship.visual.imageId = ship.prefab->imageId;
}


void PlayerShip::Destroy()
{
	if (! hasShield)
	{
		state = State::dead;
	}
}


Collider GetCollisionArea(PlayerShip& ship)
{
	return { &ship, ColliderId::player, ship.prevPos, ship.pos, ship.size };
}


void PlayerShip::SetSpeedBoost(float value)
{
	assert(value > 0);
	speedBoost = value;
	powerUpTimer = 10.f;
}


void PlayerShip::SetFireBoost(float boost)
{
	assert(boost > 0.f);
	fireBoost = 1.f / boost; // it decrease interval between shots
	powerUpTimer = 10.f; // Note specified in the rules, I'm giving the fire boost a fixed amount of time too
}


void PlayerShip::SetDoubleFire()
{
	doubleFire = true;
	tripleFire = false;
	powerUpTimer = 10.f;
}


void PlayerShip::SetTripleFire()
{
	tripleFire = true;
	doubleFire = false;
	powerUpTimer = 10.f;
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


