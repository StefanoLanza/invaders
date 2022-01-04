#include "PlayField.h"
#include "Player.h"
#include "Laser.h"
#include "Alien.h"
#include "GameData.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "Wall.h"
#include "GameConfig.h"
#include <engine/Console.h>
#include <engine/Utils.h>
#include <engine/MessageLog.h>
#include "Images.h"
#include "AIModule.h"
#include <algorithm>
#include <cassert>
#include <functional>


PlayField::PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog) : 
	rGen { rGen },
	messageLog { messageLog },
	config { config },
	bounds { iBounds },
	rndFloat01 { 0.f, 1.f },
	rndPowerUp { (int)PowerUp::count, rGen },
	availableAlienLasers(0),
	availablePlayerLasers(0)
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


void PlayField::SpawnPlayerLaser(const Laser& laser)
{
	if (availablePlayerLasers > 0)
	{
		availablePlayerLasers--;
		lasers.push_back(laser);
	}
}

//
void PlayField::SpawnAlienLaser(const Laser& laser)
{
	if (availableAlienLasers > 0)
	{
		availableAlienLasers--;
		lasers.push_back(laser);
	}
}

void PlayField::AddPlayerShip(const PlayerShip& playerShip)
{
	players.push_back(playerShip);
}

void PlayField::AddAlienShip(const Alien& alienShip)
{
	aliens.push_back(alienShip);
}

void PlayField::AddExplosion(const Vector2D& position, float timer, float delay)
{
	explosions.push_back( NewExplosion(position, timer, delay) );
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
		{ GameImageId::sPowerUp, Color::yellowIntense, },
		{ GameImageId::fPowerUp, Color::yellowIntense, },
		{ GameImageId::dPowerUp, Color::yellowIntense, },
		{ GameImageId::tPowerUp, Color::yellowIntense, },
		{ GameImageId::iPowerUp, Color::yellowIntense, },
		{ GameImageId::bomb,     Color::yellowIntense, },
	};
	powerUps.push_back( NewPowerUp(position, visuals[type], { 5.f, 3.f }, config.powerUpVelocity, (PowerUp::Type)type) );
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
		AlienHit(alien);
	}
}


void PlayField::AddWall(const Vector2D& position)
{
	walls.push_back(NewWall(position, config.wallMaxHits));
}


void PlayField::DeletePlayers()
{
	players.clear();
}
	

void PlayField::GetRenderItems(std::vector<RenderItem>& ritems)
{
	ritems.reserve(players.size() + aliens.size() + lasers.size() + explosions.size() );
	ritems.clear();

	for (const auto& wall : walls)
	{
		ritems.push_back( { wall.pos, wall.visual } );
	}
	for (const auto& player : players)
	{
		ritems.push_back( { player.pos, player.visual } );
		if (player.hasShield)
		{
			Color shieldColor = std::sin(20.f * player.accumTime) > 0.f ? Color::yellowIntense : Color::yellow;
			ritems.push_back( { Add(player.pos, { 0, -2, }), { GameImageId::shield, shieldColor } } );
		}
	}
	for (const auto& alienShip : aliens)
	{
		ritems.push_back( AlienGetRenderItem(alienShip));
	}
	for (const auto& laser : lasers)
	{
		ritems.push_back( { laser.body.pos, laser.visual } );
	}
	for (const auto& explosion : explosions)
	{
		ritems.push_back( { explosion.pos, explosion.visual } );
	}
	for (const auto& powerUp : powerUps)
	{
		ritems.push_back( { powerUp.pos, powerUp.visual } );
	}
}


const Vector2D& PlayField::GetBounds() const
{
	return bounds;
}


void PlayField::Update(float dt, const AIModule& aiModule)
{
	// Update all waves
	const ScriptArgs scriptArgs = { dt, nullptr, this, &config };

	// First move all game objects
	for (auto& player : players)
	{
		Move(player,dt, bounds, *this, config);
	}
	for (auto& powerUp : powerUps)
	{
		PowerUpMove(powerUp, dt, bounds);
	}
	for (auto& laser : lasers)
	{
		MoveLaser(laser, dt, bounds);
	}
	for (auto& alienShip : aliens)
	{
		AlienUpdate(alienShip, dt, *this, config);
	}
	if (aiModule.alienScript)
	{
		for (auto& alien : aliens)
		{
			aiModule.alienScript(alien, scriptArgs);
		}
	}

	// Loop over game objects and delete dead ones. The "swap and pop_back" technique is used to delete items
	// in O(1) as their order is not important.
	Utils::RemoveElements(players, 
		[](PlayerShip& player){ return player.state == PlayerShip::State::dead; } );
	Utils::RemoveElements(lasers, 
		[this](Laser& laser)
		{
			const bool res = laser.state == Laser::State::dead;
			if (res && laser.ownerId == -1) ++availableAlienLasers; else ++availablePlayerLasers; 
			return res;
		} );
	Utils::RemoveElements(aliens, 
		[](Alien& alien){ return alien.state == Alien::State::dead; } );
	Utils::RemoveElements(powerUps, 
		[](PowerUp& powerUp){ return powerUp.state == PowerUp::State::dead; } );
	Utils::RemoveElements(walls, 
		[](const Wall& wall){ return wall.state == Wall::State::dead; } );
	// Update and delete explosions in the same loop
	Utils::RemoveElements(explosions, [=](Explosion& explosion){ return UpdateExplosion(explosion, dt); } );
}


void PlayField::AddScore(int score, int playerId)
{
	for (auto& player : players)
	{ 
		if (player.id == playerId)
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
	lasers.clear();
	explosions.clear();
	powerUps.clear();
	walls.clear();
	alienWaves.clear();
}


void PlayField::DestroyWalls()
{
	walls.clear();
}


void PlayField::DestroyAllLasers()
{
	lasers.clear();
}


void PlayField::DestroyAllExplosions()
{
	explosions.clear();
}

int PlayField::GetAvailablePlayerLasers() const 
{
	return availablePlayerLasers; 
}
