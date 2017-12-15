#include "stdafx.h"
#include "PlayField.h"
#include "Player.h"
#include "PlayerLaser.h"
#include "Alien.h"
#include "AlienLaser.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "Wall.h"
#include "GameConfig.h"
#include "Renderer.h"
#include "Utils.h"
#include "MessageLog.h"
#include "Images.h"
#include "VisualManager.h"
#include "CollisionSpace.h"
#include <algorithm>
#include <cassert>


PlayField::PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog) : 
	bounds { iBounds },
	config { config },
	rGen { rGen },
	messageLog { messageLog },
	rndFloat01 { 0.f, 1.f },
	rndPowerUp { (int)PowerUp::count, rGen },
	visualManager { std::make_unique<VisualManager>() },
	collisionSpace { std::make_unique<CollisionSpace>() }
{}


PlayField::~PlayField() = default;


const std::vector<PlayerShip>& PlayField::GetPlayers() const
{
	return players;
}


bool PlayField::NoAliens() const
{
	return aliens.empty();
}


void PlayField::Restart()
{
	availableAlienLasers = config.maxAlienLasers;
	availablePlayerLasers = config.maxPlayerLasers * (int)players.size();
	rndPowerUp.Reset();
}


void PlayField::SpawnPlayerLaser(const PlayerLaser& laser)
{
	if (availablePlayerLasers > 0)
	{
		availablePlayerLasers--;
		playerLasers.push_back(laser);
	}
}

//
void PlayField::SpawnAlienLaser(const AlienLaser& laser)
{
	if (availableAlienLasers > 0)
	{
		availableAlienLasers--;
		alienLasers.push_back(laser);
	}
}

void PlayField::AddPlayerShip(const PlayerShip& playerShip)
{
	players.push_back(std::move(playerShip));
}

void PlayField::AddAlienShip(const Alien& alienShip)
{
	aliens.push_back(alienShip);
}

void PlayField::AddExplosion(const Vector2D& position, float timer, float delay)
{
	explosions.emplace_back(position, timer, delay);
}


void PlayField::SpawnRandomPowerUp(const Vector2D& position)
{
	// Randomly choose power up
	const int type = rndPowerUp.Next();
	SpawnPowerUp(position, type);
}


void PlayField::SpawnPowerUp(const Vector2D& position, int type)
{
	constexpr int c = (int)PowerUp::count;
	constexpr Visual visuals[c] =
	{
		{ ImageId::sPowerUp, Color::yellowIntense, },
		{ ImageId::fPowerUp, Color::yellowIntense, },
		{ ImageId::dPowerUp, Color::yellowIntense, },
		{ ImageId::tPowerUp, Color::yellowIntense, },
		{ ImageId::iPowerUp, Color::yellowIntense, },
		{ ImageId::bomb,     Color::yellowIntense, },
	};
	powerUps.push_back( PowerUp { position, visuals[type], { 5.f, 3.f }, config.powerUpVelocity, (PowerUp::Type)type } );
}


void PlayField::SpawnBomb()
{
	for (int i = 0; i < 60; ++i)
	{
		float x = 10.f + (bounds.x - 20.f) * rndFloat01(rGen);
		float y = 10.f + (bounds.y - 20.f) * rndFloat01(rGen);
		float delay = (float)i * .05f;
		AddExplosion( { x, y }, config.explosionTimer, delay);
	}
	// All aliens take damage
	for (auto& alien : aliens)
	{
		alien.DecreaseHealth();
	}
}


void PlayField::AddWall(const Vector2D& position)
{
	walls.emplace_back(position, config.wallMaxHits);
}


void PlayField::EndGame()
{
	players.clear();
}
	

void PlayField::GetRenderItems(std::vector<RenderItem>& ritems)
{
	ritems.reserve(players.size() + aliens.size() + playerLasers.size() + alienLasers.size() + explosions.size() );
	ritems.clear();

	for (const auto& wall : walls)
	{
		ritems.push_back( { wall.GetPosition(), wall.GetVisual() } );
	}
	for (const auto& player : players)
	{
		ritems.push_back( { player.GetPosition(), player.GetVisual() } );
	}
	for (const auto& alienShip : aliens)
	{
		ritems.push_back( { alienShip.GetPosition(), alienShip.GetVisual() } );
	}
	for (const auto& laser : playerLasers)
	{
		ritems.push_back( { laser.GetPosition(), laser.GetVisual() } );
	}
	for (const auto& laser : alienLasers)
	{
		ritems.push_back( { laser.GetPosition(), laser.GetVisual() } );
	}
	for (const auto& explosion : explosions)
	{
		ritems.push_back( { explosion.GetPosition(), explosion.GetVisual() } );
	}
	for (const auto& powerUp : powerUps)
	{
		ritems.push_back( { powerUp.GetPosition(), powerUp.GetVisual() } );
	}
}


const Vector2D& PlayField::GetBounds() const
{
	return bounds;
}


void PlayField::Update(float dt)
{
	// First move all game objects
	for (auto& player : players)
	{
		player.Move(dt, bounds);
	}
	for (auto& alienShip : aliens)
	{
		alienShip.Move(dt, bounds);
	}
	for (auto& powerUp : powerUps)
	{
		powerUp.Move(dt, bounds);
	}
	for (auto& laser : playerLasers)
	{
		laser.Move(dt, bounds);
	}
	for (auto& laser : alienLasers)
	{
		laser.Move(dt, bounds);
	}

	for (auto& alienShip : aliens)
	{
		alienShip.Update(dt, *this, config);
	}

	if (aliens.empty() == false)
	{
		for (auto& player : players)
		{
			player.ShootLasers(dt, *this, config.playerLaserVelocity, config.playerFireRate);
		}
	}

	// Collision detection
	collisionSpace->Clear();
	for (auto& player : players)
	{
		collisionSpace->Add(player.GetCollisionArea());
	}
	for (auto& alien : aliens)
	{
		collisionSpace->Add(alien.GetCollisionArea());
	}
	for (auto& playerLaser : playerLasers)
	{
		collisionSpace->Add(playerLaser.GetCollisionArea());
	}
	for (auto& alienLaser : alienLasers)
	{
		collisionSpace->Add(alienLaser.GetCollisionArea());
	}
	for (auto& powerUp : powerUps)
	{
		collisionSpace->Add(powerUp.GetCollisionArea());
	}
	for (auto& wall : walls)
	{
		collisionSpace->Add(wall.GetCollisionArea());
	}
	CheckCollisions();

	const size_t numAlienLasers = alienLasers.size();
	const size_t numPlayerLasers = playerLasers.size();

	// Loop over game objects and delete dead ones. The "swap and pop_back" technique is used to delete items
	// in O(1) as their order is not important.
	Utils::RemoveElements(players, [](PlayerShip& player){ return player.GetState() == PlayerShip::State::dead; } );
	Utils::RemoveElements(playerLasers, [](PlayerLaser& laser){ return laser.GetState() == PlayerLaser::State::dead; } );
	Utils::RemoveElements(alienLasers, [](AlienLaser& laser){ return laser.GetState() == AlienLaser::State::dead; } );
	Utils::RemoveElements(aliens, [](Alien& alien){ return alien.GetState() == Alien::State::dead; } );
	Utils::RemoveElements(powerUps, [](PowerUp& powerUp){ return powerUp.GetState() == PowerUp::State::dead; } );
	Utils::RemoveElements(walls, [](Wall& wall){ return wall.GetState() == Wall::State::dead; } );
	// Update and delete explosions in the same loop
	Utils::RemoveElements(explosions, [=](Explosion& explosion){ return explosion.Update(dt); } );

	availablePlayerLasers += (int)(numPlayerLasers - playerLasers.size());
	availableAlienLasers += (int)(numAlienLasers - alienLasers.size());
}


void PlayField::CheckCollisions()
{
	CollisionInfo collisions[64];
	const int nc = collisionSpace->Execute(collisions, _countof(collisions));
	CollisionInfo* c = collisions;
	for (int i = 0; i < nc; ++i, ++c)
	{
		// Collision matrix
		if (c->id0 == CollisionId::player && c->id1 == CollisionId::alienLaser)
		{
			Collision_PlayerVSLaser(c->ud0, c->ud1);
		}
		else if (c->id0 == CollisionId::player && c->id1 == CollisionId::powerUp)
		{
			Collision_PlayerVSPowerUp(c->ud0, c->ud1);
		}
		else if (c->id0 == CollisionId::alien && c->id1 == CollisionId::playerLaser)
		{
			Collision_AlienVSLaser(c->ud0, c->ud1);
		}
		else if (c->id0 == CollisionId::alien && c->id1 == CollisionId::wall)
		{
			Collision_AlienVSWall(*static_cast<Alien*>(c->ud0), *static_cast<Wall*>(c->ud1));
		}
		else if (c->id0 == CollisionId::playerLaser && c->id1 == CollisionId::alienLaser)
		{
			Collision_LaserVSLaser(*static_cast<PlayerLaser*>(c->ud0), *static_cast<AlienLaser*>(c->ud1));
		}
		else if (c->id0 == CollisionId::playerLaser && c->id1 == CollisionId::alien)
		{
			Collision_PlayerVSAlien(c->ud0, c->ud1);
		}
		else if (c->id0 == CollisionId::playerLaser && c->id1 == CollisionId::wall)
		{
			Collision_LaserVSWall(*static_cast<PlayerLaser*>(c->ud0), *static_cast<Wall*>(c->ud1));
		}
	}
}


void PlayField::Collision_LaserVSLaser(PlayerLaser& playerLaser, AlienLaser& alienLaser)
{
	// Spawn explosion, kill this and the alien laser
	Vector2D pos = Lerp(playerLaser.GetPosition(), alienLaser.GetPosition(), 0.5f);
	AddExplosion(pos, config.explosionTimer);
	alienLaser.Destroy();
	playerLaser.Destroy();
}


void PlayField::Collision_AlienVSLaser(void* ud0, void* ud1)
{
	Alien& alien = *(Alien*)ud0;
	PlayerLaser& playerLaser = *(PlayerLaser*)ud1;
	playerLaser.Destroy();
	alien.DecreaseHealth(); // kill the alien that we hit
	//Spawn explosion 
	AddScore( alien.GetState() == Alien::State::normal ? 10 : 20, playerLaser.GetPlayerId());
	AddExplosion(alien.GetPosition(), config.explosionTimer);
	if (alien.GetState() == Alien::State::dead)
	{
		if (rndFloat01(rGen) < config.powerUpRate)
		{
			SpawnRandomPowerUp(alien.GetPosition());
		}
	}

	// FIXME
	static int b = 0; if (!b++) {
	SpawnPowerUp( { bounds.x/2, 10.f }, PowerUp::Type::bomb);
	SpawnPowerUp( { bounds.x/2 - 10, 10.f }, PowerUp::Type::doubleFire);
}
}


void PlayField::Collision_PlayerVSLaser(void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	AlienLaser& alienLaser = *(AlienLaser*)ud1;
	//Spawn explosion, destroy player and laser
	alienLaser.Destroy();
	if (! config.godMode)
	{
		player.Destroy();
	}
	AddExplosion(player.GetPosition(), config.explosionTimer);
}


void PlayField::Collision_PlayerVSAlien(void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	Alien& alien = *(Alien*)ud1;
	//Spawn explosion, destroy player and alien
	alien.Destroy();
	if (! config.godMode)
	{
		player.Destroy();
	}
	AddExplosion(alien.GetPosition(), config.explosionTimer);
}


void PlayField::Collision_PlayerVSPowerUp(void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	PowerUp& powerUp = *(PowerUp*)ud1;
	ActivatePowerUp(player, powerUp, messageLog);
	powerUp.Destroy();
}


void PlayField::Collision_LaserVSWall(PlayerLaser& playerLaser, Wall& wall)
{
	playerLaser.Destroy();
	wall.Hit();
	const Vector2D pos = { wall.GetPosition().x,  wall.GetPosition().y + 1.f };
	AddExplosion(pos, config.explosionTimer);
}


void PlayField::Collision_AlienVSWall(Alien& alien, Wall& wall)
{
	alien.AvoidWall(wall.GetPosition());
}


void PlayField::ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog)
{
	switch (powerUp.GetType())
	{
	case PowerUp::speedBoost:
		messageLog.AddMessage("Speed Boost!");
		player.SetSpeedBoost(2);
		break;
	case PowerUp::fireBoost:
		messageLog.AddMessage("Fire Boost!");
		player.SetFireBoost(2);
		break;
	case PowerUp::doubleFire:
		messageLog.AddMessage("Double Fire!");
		player.SetDoubleFire();
		break;
	case PowerUp::tripleFire:
		messageLog.AddMessage("Triple Fire!");
		player.SetTripleFire();
		break;
	case PowerUp::invulnerability:
		messageLog.AddMessage("Invulnerability!");
		player.SetInvulnerable(config.powerUpInvulnerabilityTime);
		break;
	case PowerUp::bomb:
		messageLog.AddMessage("Bomb!");
		SpawnBomb();
		break;
	}
}


void PlayField::AddScore(int score, int playerId)
{
	for (auto& player : players)
	{ 
		if (player.GetId() == playerId)
		{
			player.AddScore(score);
			break;
		}
	}
}


void PlayField::DestroyAll()
{
	players.clear();
	aliens.clear();
	playerLasers.clear();
	alienLasers.clear();
	explosions.clear();
	powerUps.clear();
	walls.clear();
	visualManager->Clear();
}


void PlayField::DestroyWalls()
{
	walls.clear();
}


int PlayField::GetAvailablePlayerLasers() const 
{
	return availablePlayerLasers; 
}
