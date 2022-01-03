#include "Scripts.h"
#include <game/Alien.h>
#include <game/GameData.h>
#include <game/PlayField.h>
#include <game/Laser.h>
#include <game/GameConfig.h>
#include <game/Images.h>
#include <random>
#include <algorithm>
#include <cassert>


namespace
{

bool AlienCanShoot(const Alien& alien, const AlienWave& wave) {
	int col = alien.indexInWave % wave.numCols;
	for (int row = alien.indexInWave / wave.numCols + 1; row < wave.numRows; ++row) {
		if (wave.mask[row * 8 + col]) { // FIXME layout by col
			return false;
		}
	}
   	return true;
}


void NormalAlienScript(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	AlienWave& wave = world.alienWaves[alien.waveIndex];

	const Vector2D size = alien.body.size;
	// Border check vertical:
	if (alien.body.pos.y >= world.bounds.y - size.y)
	{
		// If an alien ship reaches the bottom of the screen the players die
		world.DeletePlayers();
	}
	if (alien.body.pos.y > world.bounds.y - size.y * 0.5f)
	{
		AlienDestroy(alien, wave);// FIXME only mark as dead
	}

	if (alien.gameState.fireTimer == 0.f)
	{ 
		// Randomly shoot lasers
		alien.gameState.fireTimer = (1.f / wave.fireRate) * (1.f + alien.randomOffset);
	}
	alien.gameState.fireTimer -= dt;
	if (alien.gameState.fireTimer < 0.f)
	{
		{ //if (AlienCanShoot(alien, wave)) {
			const Vector2D laserPos = { alien.body.pos.x, alien.body.pos.y + size.y * 0.5f }; // spawn in front
			world.SpawnAlienLaser( NewLaser(laserPos, { 0.f, gameConfig.alienLaserVelocity }, { GameImageId::alienLaser, Color::greenIntense }, -1, ColliderId::alienLaser) );
			alien.gameState.fireTimer = 0.f; // reset it
		}
	}
}

}

extern "C"
{

DLL_EXPORT void ExecuteAlienScript(Alien& alien, const ScriptArgs& args)
{
	NormalAlienScript(alien, args.dt, *args.world, *args.gameConfig);
}

}