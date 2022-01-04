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
	int col = alien.indexInWave % wave.numCols;
	for (int row = alien.indexInWave / wave.numCols + 1; row < wave.numRows; ++row) {
		if (wave.mask[row * wave.numCols + col]) {
			return false;
		}
	}
   	return true;
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
	if (alien.gameState.fireTimer < 0.f)
	{
		if (AlienCanShoot(alien, wave)) 
		{
			const Vector2D laserPos = { alien.body.pos.x, alien.body.pos.y + size.y * 0.5f }; // spawn in front
			Vector2D laserVel;
			bool shoot = true;
			if (alien.prefab->aimAtPlayer)
			{
				Vector2D diff = Sub(world.GetPlayers()[0].pos, laserPos);
				// Limit angle
				if (2 * std::abs(diff.y) > std::abs(diff.x)) 
				{
					laserVel = Normalize(diff, alien.prefab->laserSpeed);
				}
				else
				{
					shoot = false;
				}
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

}

extern "C"
{

DLL_EXPORT void ExecuteAlienScript(Alien& alien, const ScriptArgs& args)
{
	AlienScript(alien, args.dt, *args.world, *args.gameConfig);
}

}