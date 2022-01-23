#include "Scripts.h"
#include <game/Alien.h>
#include <game/GameData.h>
#include <game/PlayField.h>
#include <game/Laser.h>
#include <game/GameConfig.h>
#include <game/Images.h>
#include <game/Player.h>
#include <engine/CollisionSpace.h>
#include <random>
#include <algorithm>
#include <cassert>


namespace
{

bool AlienCanShoot(const Alien& alien, const CollisionSpace& collisionSpace) {
	Rectangle rect;
	constexpr float margin = 5.f;
	rect.v0 = Add(alien.body.pos, { -margin, alien.body.size.y});
	rect.v1 = Add(rect.v0, { margin * 2.f, 80 });
	return ! collisionSpace.TestRect(rect, ColliderId::alien);
}


bool Aim(Vector2D& laserVel, const Vector2D& laserPos, float laserSpeed, const std::vector<PlayerShip>& players)
{
	const PlayerShip* targetPlayer = nullptr;
	float mind = std::numeric_limits<float>::max();
	for (const auto& player : players)
	{
		if (float d = std::abs(player.body.pos.x - laserPos.x); d < mind)
		{
			mind = d;
			targetPlayer = &player;
		}
	}
	if (nullptr == targetPlayer)
	{
		return false;
	}
	Vector2D diff = Sub(targetPlayer->body.pos, laserPos);
	// Limit angle
	if (2 * std::abs(diff.y) > std::abs(diff.x)) 
	{
		laserVel = Normalize(diff, laserSpeed);
		return true;
	}
	else
	{
		return false;
	}
}

void AlienScript(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig, const CollisionSpace& collisionSpace)
{
	if (alien.state != Alien::State::attacking)
	{
		return;
	}
	AlienWave& wave = world.alienWaves[alien.waveIndex];

	const Vector2D size = alien.body.size;
	// Border check vertical:
	if (alien.body.pos.y >= world.bounds.y - size.y * 0.5f)
	{
		// If an alien ship reaches the bottom of the screen the players die
		world.KillPlayers();
		AlienDestroy(alien);
	}

	alien.gameState.fireTimer -= dt;
	if (alien.gameState.fireTimer < 0.f) 
	{
		if (AlienCanShoot(alien, collisionSpace))
		{
			const Vector2D laserPos = { alien.body.pos.x, alien.body.pos.y + size.y * 0.5f }; // spawn in front
			Vector2D laserVel;
			bool shoot = true;
			if (alien.prefab->aimAtPlayer)
			{
				shoot = Aim(laserVel, laserPos, alien.prefab->laserSpeed, world.GetPlayers());
			}
			else 
			{
				laserVel = { 0.f, alien.prefab->laserSpeed };
			}
			if (shoot)
			{
				world.AddLaser( NewLaser(laserPos, laserVel, {GameImageId::alienLaser, Color::greenIntense}, -1, ColliderId::alienLaser));
			}
		}
		alien.gameState.fireTimer += (1.f / alien.prefab->fireRate); // reset it
	}
}

}

extern "C"
{

DLL_EXPORT void ExecuteAlienScript(Alien& alien, const ScriptArgs& args)
{
	AlienScript(alien, args.dt, *args.world, *args.gameConfig, *args.collisionSpace);
}

}