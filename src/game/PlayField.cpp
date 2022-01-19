#include "PlayField.h"
#include "Player.h"
#include "Laser.h"
#include "Alien.h"
#include "GameData.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "Wall.h"
#include "Particle.h"
#include "GameConfig.h"
#include <engine/Console.h>
#include <engine/Utils.h>
#include "Images.h"
#include <algorithm>
#include <cassert>
#include <functional>


PlayField::PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen) : 
	rGen { rGen },
	config { config },
	bounds { iBounds },
	rndFloat01 { 0.f, 1.f },
	rndPowerUp { (int)PowerUp::count, rGen }
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


bool PlayField::NoParticles() const
{
	return particles.empty();
}

void PlayField::Restart()
{
	rndPowerUp.Reset();
}


void PlayField::SpawnPlayerLaser(const Laser& laser)
{
	lasers.push_back(laser);
}

//
void PlayField::SpawnAlienLaser(const Laser& laser)
{
	lasers.push_back(laser);
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
	constexpr Visual visuals[] =
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
		if (AlienHit(alien))
		{
			AlienDestroy(alien);
		}
	}
}


void PlayField::AddWall(const Vector2D& position)
{
	walls.push_back(NewWall(position, config.wallMaxHits));
}


void PlayField::KillPlayers()
{
	for (PlayerShip& player : players)
	{
		PlayerHit(player);
	}
}
	

void PlayField::GetRenderItems(std::vector<RenderItem>& ritems)
{
	ritems.reserve(stars.size() + particles.size() + walls.size() + players.size() + aliens.size() + lasers.size() + explosions.size() + powerUps.size() );
	ritems.clear();

	for (const Star& star : stars)
	{
		RenderItem ritem;
		ritem.pos = star.pos;
		ritem.visual = { GameImageId::particle, star.t > 0.5f ? Color::whiteIntense : Color::white };
		ritems.push_back(ritem);
	}

	// Note: order is important. Latter elements cover the former ones on screen
	for (const auto& particle : particles)
	{
		ritems.push_back( { particle.pos, { GameImageId::particle, particle.color }});
	}
	for (const auto& wall : walls)
	{
		ritems.push_back( { wall.pos, wall.visual } );
	}
	for (const auto& player : players)
	{
		if (player.visible)
		{
			ritems.push_back( { player.body.pos, player.visual } );
			if (player.hasShield)
			{
				ritems.push_back( { Add(player.body.pos, { 0, -2, }), { GameImageId::shield, player.shieldColor } } );
			}
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

void PlayField::RemoveDead()
{
	// Loop over game objects and delete dead ones. The "swap and pop_back" technique is used to delete items
	// in O(1) as their order is not important.
	Utils::RemoveElements(players, 
		[](const PlayerShip& player){ return player.state == PlayerShip::State::dead; } );
	Utils::RemoveElements(lasers, 
		[this](const Laser& laser)
		{
			return laser.state == Laser::State::dead;
		} );
	Utils::RemoveElements(aliens, 
		[](const Alien& alien){ return alien.state == Alien::State::dead; } );
	Utils::RemoveElements(powerUps, 
		[](const PowerUp& powerUp){ return powerUp.state == PowerUp::State::dead; } );
	Utils::RemoveElements(walls, 
		[](const Wall& wall){ return wall.state == Wall::State::dead; } );
	Utils::RemoveElements(explosions, [=](const Explosion& explosion){ return IsExplosionOver(explosion); });
	Utils::RemoveElements(particles, [=](const Particle& particle){ return particle.life < 0; });
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
	particles.clear();
	stars.clear();
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

