#include "Scripts.h"
#include <game/Alien.h>
#include <game/GameData.h>
#include <game/PlayField.h>
#include <game/Laser.h>
#include <game/GameConfig.h>
#include <game/Images.h>
#include <game/Player.h>
#include <random>
#include <algorithm>
#include <cassert>


namespace
{

bool AlienCanShoot(const Alien& alien, const AlienWave& wave) {
	const int col = alien.indexInWave % wave.numCols;
	for (int row = alien.indexInWave / wave.numCols + 1; row < wave.numRows; ++row) {
		if (wave.collisionMask[row * wave.numCols + col]) {
			return false;
		}
	}
   	return true;
}


bool Aim(Vector2D& laserVel, const Vector2D& laserPos, float laserSpeed, const std::vector<PlayerShip>& players)
{
	const PlayerShip* targetPlayer = nullptr;
	float mind = std::numeric_limits<float>::max();
	for (const auto& player : players)
	{
		if (float d = std::abs(player.pos.x - laserPos.x); d < mind)
		{
			mind = d;
			targetPlayer = &player;
		}
	}
	assert(targetPlayer);
	Vector2D diff = Sub(targetPlayer->pos, laserPos);
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

void AlienScript(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	AlienWave& wave = world.alienWaves[alien.waveIndex];

	const Vector2D size = alien.body.size;
	// Border check vertical:
	if (alien.body.pos.y >= world.bounds.y - size.y * 0.5f)
	{
		// If an alien ship reaches the bottom of the screen the players die
		world.DeletePlayers();
		AlienDestroy(alien, wave);// FIXME only mark as dead
	}

	if (alien.gameState.fireTimer == 0.f)
	{ 
		// Randomly shoot lasers
		alien.gameState.fireTimer = (1.f / (alien.prefab->fireRate + wave.fireRate)) * (1.f + alien.randomOffset);
	}
	alien.gameState.fireTimer -= dt;
	if (alien.gameState.fireTimer < 0.f && AlienCanShoot(alien, wave)) 
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
			world.SpawnAlienLaser( NewLaser(laserPos, laserVel, {GameImageId::alienLaser, Color::greenIntense}, -1, ColliderId::alienLaser));
			alien.gameState.fireTimer = 0.f; // reset it
		}
	}
}

}

extern "C"
{

DLL_EXPORT void ExecuteAlienScript(Alien& alien, const ScriptArgs& args)
{
	AlienScript(alien, args.dt, *args.world, *args.gameConfig);
}

}