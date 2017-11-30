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
#include <algorithm>
#include <cassert>


PlayField::PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen) : 
	bounds { iBounds },
	config { config },
	rGen { rGen },
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

void PlayField::AddExplosion(const Vector2D& position, float timer)
{
	explosions.emplace_back(position, timer);
}

void PlayField::SpawnPowerUp(const Vector2D& position)
{
	constexpr int c = (int)PowerUp::count;
	constexpr Visual visuals[c] =
	{
		{ ImageId::sPowerUp, Color::yellowIntense, },
		{ ImageId::fPowerUp, Color::yellowIntense, },
		{ ImageId::tPowerUp, Color::yellowIntense, },
		{ ImageId::iPowerUp, Color::yellowIntense, },
	};
	// Randomly choose power up
	std::uniform_int_distribution<int> rndInt(0, c - 1);
	const int type = rndPowerUp.Next();
	powerUps.push_back( PowerUp { position, visuals[type], { 5.f, 3.f }, config.powerUpVelocity, (PowerUp::Type)type } );
}


void PlayField::AddWall(const Vector2D& position)
{
	walls.emplace_back(position, config.wallMaxHits);
}


void PlayField::EndGame()
{
	players.clear();
}
	

/*
GetRenderItems re builds the list of render items every frame which might degrade performance for huge numbers of game objects. The code below is also repetitive and 
does not handle automatically new game objects types.
An alternative approach would be to have a container of render items, with O(1) creation and deletion methods. Each game object would have a handle to a RenderItem, 
update its position when it moves and delete it when destroyed. In pseudo code:

class Player
{
	RenderItemHandle renderItem;
};


class RenderItemManager
{
	RenderItemHandle Alloc();  // O(1)
	void Free(RenderItemHandle handle); // O(1)
	void Move(RenderItemHandle handle, Vector2D newPos);
	const vector<RenderItem>& GetItems() const; // passed to Renderer

private:

	vector<RenderItem> items;
*/
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


void PlayField::Update(float dt, MessageLog& messageLog)
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
	CheckCollisions_LasersVSLasers();
	CheckCollisions_AliensVSLasers();
	CheckCollisions_PlayersVSLasers();
	CheckCollisions_PlayersVSAliens();
	CheckCollisions_PlayersVSPowerUps(messageLog);
	CheckCollisions_LasersVSWalls();
	CheckCollisions_AliensVSWalls();

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


void PlayField::CheckCollisions_LasersVSLasers()
{
	for (auto& playerLaser : playerLasers)
	{
		for (auto& alienLaser : alienLasers)
		{
			if (Intersect(playerLaser.GetCollisionArea(), alienLaser.GetCollisionArea()))
			{
				// Spawn explosion, kill this and the alien laser
				Vector2D pos = Lerp(playerLaser.GetPosition(), alienLaser.GetPosition(), 0.5f);
				AddExplosion(pos, config.explosionTimer);
				alienLaser.Destroy();
				playerLaser.Destroy();
			}
		}
	}
}

void PlayField::CheckCollisions_AliensVSLasers()
{
	for (auto& laser : playerLasers)
	{
		for (auto& alien : aliens)
		{
			if (Intersect(alien.GetCollisionArea(), laser.GetCollisionArea()))
			{
				laser.Destroy();
				alien.DecreaseHealth(); // kill the alien that we hit
				//Spawn explosion 
				AddScore( alien.GetState() == Alien::State::normal ? 10 : 20, laser.GetPlayerId());
				AddExplosion(alien.GetPosition(), config.explosionTimer);
				if (alien.GetState() == Alien::State::dead)
				{
					if (rndFloat01(rGen) < config.powerUpRate)
					{
						SpawnPowerUp(alien.GetPosition());
					}
				}
			}
		}
	}
}


void PlayField::CheckCollisions_PlayersVSLasers()
{
	for (auto& player : players)
	{
		for (auto& laser : alienLasers)
		{
			if (Intersect(player.GetCollisionArea(), laser.GetCollisionArea()))
			{
				//Spawn explosion, destroy player and laser
				laser.Destroy();
				if (! config.godMode)
				{
					player.Destroy();
				}
				AddExplosion(player.GetPosition(), config.explosionTimer);
			}
		}
	}
}


void PlayField::CheckCollisions_PlayersVSAliens()
{
	for (auto& player : players)
	{
		for (auto& alien : aliens)
		{
			if (Intersect(player.GetCollisionArea(), alien.GetCollisionArea()))
			{
				//Spawn explosion, destroy player and alien
				alien.Destroy();
				if (! config.godMode)
				{
					player.Destroy();
				}
				AddExplosion(alien.GetPosition(), config.explosionTimer);
			}
		}
	}
}


void PlayField::CheckCollisions_PlayersVSPowerUps(MessageLog& messageLog)
{
	for (auto& player : players)
	{
		for (auto& powerUp : powerUps)
		{
			if (Intersect(player.GetCollisionArea(), powerUp.GetCollisionArea()))
			{
				EnanchePlayer(player, powerUp, messageLog);
				powerUp.Destroy();
			}
		}
	}
}


void PlayField::CheckCollisions_LasersVSWalls()
{
	for (auto& laser : playerLasers)
	{
		for (auto& wall : walls)
		{
			if (Intersect(laser.GetCollisionArea(), wall.GetCollisionArea()))
			{
				laser.Destroy();
				wall.Hit();
				const Vector2D pos = { wall.GetPosition().x,  wall.GetPosition().y + 1.f };
				AddExplosion(pos, config.explosionTimer);
			}
		}
	}
}


void PlayField::CheckCollisions_AliensVSWalls()
{
	for (auto& alien : aliens)
	{
		for (const auto& wall : walls)
		{
			if (Intersect(alien.GetCollisionArea(), wall.GetCollisionArea()))
			{
				alien.AvoidWall(wall.GetPosition());
			}
		}
	}
}


void PlayField::EnanchePlayer(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog) const
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
	case PowerUp::tripleFire:
		messageLog.AddMessage("Triple Fire!");
		player.SetTripleFire(true);
		break;
	case PowerUp::invulnerability:
		messageLog.AddMessage("Invulnerability!");
		player.SetInvulnerable(config.powerUpInvulnerabilityTime);
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
}


void PlayField::DestroyWalls()
{
	walls.clear();
}


int PlayField::GetAvailablePlayerLasers() const 
{
	return availablePlayerLasers; 
}
