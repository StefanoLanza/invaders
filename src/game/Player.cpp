#include "Player.h"
#include "PlayField.h"
#include "Laser.h"
#include <engine/Input.h>
#include "Images.h"
#include "GameConfig.h"
#include "GameData.h"
#include <engine/Collision.h>
#include <random>
#include <algorithm>
#include <cassert>

void ShootLasers(PlayerShip& ship, float dt, PlayField& world, float laserVelocity, float fireRate)
{
	constexpr Visual laserVisual[3] =
	{
		{ GameImageId::playerLaser, Color::lightBlueIntense },
		{ GameImageId::playerLaserLeft, Color::lightBlueIntense },
		{ GameImageId::playerLaserRight, Color::lightBlueIntense },
	};

	// Randomly shoot laser shots
	ship.fireTimer -= dt;
	if (ship.fireTimer < 0.f)
	{
		ship.fireTimer = 1.f / (fireRate * ship.fireBoost);
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


void Move(PlayerShip& player, float dt, const Vector2D& worldBounds, PlayField& world, const GameConfig& gameConfig)
{
	// Update input state
	player.input->Update(dt);

	const float halfWidth = player.size.x / 2.f;

	player.prevPos = player.pos;
	// The original code did not handle the first and last column robustly
	if (player.input->Left())
	{
		player.pos.x -= player.prefab->velocity * player.speedBoost * dt;
		player.pos.x = std::max(halfWidth, player.pos.x);
	}
	else if (player.input->Right())
	{
		player.pos.x += player.prefab->velocity * player.speedBoost * dt;
		player.pos.x = std::min(player.pos.x, worldBounds.x - halfWidth);
	}
	if (player.input->Fire()) {
		ShootLasers(player, dt, world, gameConfig.playerLaserVelocity, gameConfig.playerFireRate);
	}

	// Boosts lasts a certain amount of time
	player.powerUpTimer -= dt;
	if (player.powerUpTimer < 0.f)
	{
		// Restore normal values
		player.powerUpTimer = 0.f;
		player.speedBoost = 1.f;
		player.fireBoost = 1.f;
		player.doubleFire = false;
		player.tripleFire = false;
	}

	player.accumTime += dt; // useful for time based effects

	player.invulnerabilityTime = std::max(0.f, player.invulnerabilityTime - dt);
	if (player.invulnerabilityTime > 3.f)
	{
		player.visual.color = player.prefab->invulnColor;
		player.hasShield = true;
	}
	else if (player.invulnerabilityTime > 0)
	{
		// Flicker player color to indicate invulnerability is about to expire
		player.visual.color = std::sin(20.f * player.accumTime) > 0.f ? player.prefab->invulnColor : player.prefab->color;
		player.hasShield = true;
	}
	else
	{
		player.visual.color = player.prefab->color;
		player.hasShield = false;
	}
	player.visual.imageId = player.prefab->imageId;
}


void PlayerDestroy(PlayerShip& player)
{
	if (! player.hasShield)
	{
		player.state = PlayerShip::State::dead;
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
	assert(boost > 1.f);
	fireBoost = boost;
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


