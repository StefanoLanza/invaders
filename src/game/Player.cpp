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

namespace
{

void PlayerShootLasers(PlayerShip& ship, float dt, PlayField& world, float laserVelocity, float fireRate)
{
	constexpr Visual laserVisual =
	{
		GameImageId::playerLaser, Color::lightBlueIntense
	};

	// Randomly shoot laser shots
	ship.fireTimer -= dt;
	if (ship.fireTimer < 0.f)
	{
		ship.fireTimer = 1.f / (fireRate * ship.fireBoost);
		const float l = laserVelocity;
		// Spawn lasers in front
		const Vector2D laserPos_l = { ship.body.pos.x - ship.prefab->laserOffset, ship.body.pos.y - ship.body.size.y * 0.5f };
		const Vector2D laserPos_r = { ship.body.pos.x + ship.prefab->laserOffset, ship.body.pos.y - ship.body.size.y * 0.5f };
		const Vector2D laserPos_m = { ship.body.pos.x, ship.body.pos.y - ship.body.size.y * 0.5f };
		if (ship.doubleFire)
		{
			world.SpawnPlayerLaser( NewLaser(laserPos_l, { 0, -l }, laserVisual, ship.id, ColliderId::playerLaser) );
			world.SpawnPlayerLaser( NewLaser(laserPos_r, { 0, -l }, laserVisual, ship.id, ColliderId::playerLaser) );
		}
		else if (ship.tripleFire)
		{
			world.SpawnPlayerLaser( NewLaser(laserPos_l, { 0, -l }, laserVisual, ship.id, ColliderId::playerLaser) );
			world.SpawnPlayerLaser( NewLaser(laserPos_r, { 0, -l }, laserVisual, ship.id, ColliderId::playerLaser) );
			world.SpawnPlayerLaser( NewLaser(laserPos_m, { 0, -l }, laserVisual, ship.id, ColliderId::playerLaser) );
		}
		else // single fire
		{
			world.SpawnPlayerLaser( NewLaser(laserPos_m, { 0, -l }, laserVisual, ship.id, ColliderId::playerLaser) );  // straight
		}
		++ship.laserShots;
	}
}


void ClearPowerUps(PlayerShip& player)
{
	// Restore normal values
	player.powerUpTimer = 0.f;
	player.speedBoost = 1.f;
	player.fireBoost = 1.f;
	player.doubleFire = false;
	player.tripleFire = false;
}

}


PlayerShip NewPlayerShip(const Vector2D& initialPos, const PlayerPrefab& prefab, int id, std::shared_ptr<Input> input_)
{
	PlayerShip player;
	player.body.pos = initialPos;
	player.body.prevPos = initialPos;
	player.body.size = GetImageSize(prefab.imageId);
	player.prefab = &prefab;
	player.id = id;
	player.fireTimer = 0.f;
	player.fireBoost = 1.f;
	player.state = PlayerShip::State::normal;
	player.speedBoost = 1.f;
	player.score = 0;
	player.input = std::move(input_);
	player.powerUpTimer = 0.f;
	player.shieldTime = 0.f;
	player.invulnerabilityTime = 0.f;
	player.accumTime = 0.f;
	player.laserShots = 0;
	player.lives = 3;
	player.doubleFire = false;
	player.tripleFire = false;
	player.hasShield = false;
	player.visible = true;
	player.shieldColor = Color::yellowIntense;
	player.visual.imageId = player.prefab->imageId;
	return player;
}


void Move(PlayerShip& player, float dt, const Vector2D& worldBounds, PlayField& world, const GameConfig& gameConfig)
{
	if (player.state == PlayerShip::State::dead)
	{
		return;
	}
	
	// Update input state
	player.input->Update(dt);

	const float halfWidth = player.body.size.x / 2.f;

	player.body.prevPos = player.body.pos;
	// The original code did not handle the first and last column robustly
	if (player.input->Left())
	{
		player.body.pos.x -= player.prefab->velocity * player.speedBoost * dt;
		player.body.pos.x = std::max(halfWidth, player.body.pos.x);
	}
	else if (player.input->Right())
	{
		player.body.pos.x += player.prefab->velocity * player.speedBoost * dt;
		player.body.pos.x = std::min(player.body.pos.x, worldBounds.x - halfWidth);
	}
	if (player.input->Fire()) 
	{
		PlayerShootLasers(player, dt, world, gameConfig.playerLaserVelocity, gameConfig.playerFireRate);
	}

	// Boosts lasts a certain amount of time
	player.powerUpTimer -= dt;
	if (player.powerUpTimer < 0.f)
	{
		ClearPowerUps(player);
	}

	player.accumTime += dt; // useful for time based effects

	// Invulnerability when just hit
	if (player.state == PlayerShip::State::justHit)
	{
		player.invulnerabilityTime = std::max(0.f, player.invulnerabilityTime - dt);
		if (player.invulnerabilityTime > 0.f)
		{
			// Flicker player to indicate invulnerability is about to expire
			player.visible = std::sin(40.f * player.accumTime) > 0.f ? true : false;
		}
		else
		{
			player.visible = true;
			player.state = PlayerShip::State::normal;
		}
	}

	// Shield
	player.shieldTime = std::max(0.f, player.shieldTime - dt);
	if (player.shieldTime > 3.f)
	{
		player.shieldColor = Color::yellowIntense;
		player.hasShield = true;
	}
	else if (player.shieldTime > 0.f)
	{
		// Flicker shield color to indicate invulnerability is about to expire
		player.shieldColor = std::sin(40.f * player.accumTime) > 0.f ? Color::yellowIntense : Color::black;
		player.hasShield = true;
	}
	else
	{
		player.hasShield = false;
	}
}


void PlayerHit(PlayerShip& player)
{
	if (! player.hasShield && player.state == PlayerShip::State::normal)
	{
		assert(player.lives > 0);
		--player.lives;
		if (player.lives == 0)
		{
			player.state = PlayerShip::State::dead;
		}
		else 
		{
			player.state = PlayerShip::State::justHit;
		    player.SetInvulnerable(4.f);
		}
	}
}


void KillPlayer(PlayerShip& player)
{
	player.state = PlayerShip::State::dead;
}


Collider GetCollisionArea(PlayerShip& ship)
{
	return { &ship, ColliderId::player, ship.body.prevPos, ship.body.pos, ship.body.size };
}


void PlayerShip::SetSpeedBoost(float value)
{
	assert(value > 0);
	ClearPowerUps(*this);
	speedBoost = value;
	fireBoost = 1.f;
	powerUpTimer = 10.f;
}


void PlayerShip::SetFireBoost(float boost)
{
	assert(boost > 1.f);
	ClearPowerUps(*this);
	fireBoost = boost;
	powerUpTimer = 10.f; // Note specified in the rules, I'm giving the fire boost a fixed amount of time too
}


void PlayerShip::SetDoubleFire()
{
	ClearPowerUps(*this);
	doubleFire = true;
	powerUpTimer = 5.f;
}


void PlayerShip::SetTripleFire()
{
	ClearPowerUps(*this);
	tripleFire = true;
	powerUpTimer = 5.f;
}


void PlayerShip::SetInvulnerable(float timer)
{
	invulnerabilityTime = timer;
}


void PlayerShip::SetShield(float time) 
{
	shieldTime = time;
}

void PlayerShip::AddScore(int increment)
{
	assert(increment > 0);
	score += increment;
}


