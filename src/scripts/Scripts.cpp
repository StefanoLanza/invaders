#include "Scripts.h"
#include <game/Alien.h>
#include <game/Prefabs.h>
#include <game/PlayField.h>
#include <game/Laser.h>
#include <game/GameConfig.h>
#include <game/Images.h>
#include <random>
#include <algorithm>
#include <cassert>


namespace
{

void MoveReflect(Body& body, AlienWave& wave, float dt, const Vector2D& worldBounds)
{
	const float halfWidth = body.size.x * 0.5f;
	body.prevPos = body.pos;
	body.velocity.x = std::abs(body.velocity.x) * wave.direction;
	body.pos = Mad(body.pos, body.velocity, dt);

	// Border check
	if (body.pos.x < halfWidth)
	{
		body.pos.x = halfWidth;
		//body.pos.y += body.size.y;
		body.velocity.x = std::abs(body.velocity.x);
		wave.direction = 1;
	}
	else if (body.pos.x > worldBounds.x - halfWidth)
	{
		body.pos.x = (float)worldBounds.x - halfWidth;
		body.velocity.x = -std::abs(body.velocity.x);
		wave.direction = -1;
		//body.pos.y += body.size.y;
	}
}

void AlienSetFireTimer(Alien& alien, const GameConfig& gameConfig, std::uniform_real_distribution<float>& rndFloat01, std::default_random_engine& rGen)
{
	alien.gameState.fireTimer = alien.state == Alien::State::normal ? gameConfig.alienFireRate : gameConfig.betterAlienFireRate;
	// Randomize it
	alien.gameState.fireTimer *= (1.f + rndFloat01(rGen));
}


void AlienTransformIntoBetter(Alien& alien)
{
	alien.state = Alien::State::better;
	alien.animState.frame = 0;
	const float sign = alien.body.velocity.x > 0.f ? +1.f : -1.f;
	alien.body.velocity.x = alien.betterPrefab->speed * sign;
	alien.gameState.health = alien.betterPrefab->health;
}


bool AlienCanShoot(const Alien& alien, const AlienWave& wave) {
	int col = alien.indexInWave % 8;
	for (int row = alien.indexInWave / 8 + 1; row < 4; ++row) {
		if (wave.mask[row * 8 + col]) { // FIXME layout by col
			return false;
		}
	}
   	return true;
}

void AlienScript(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{
	AlienWave& wave = world.alienWaves[alien.waveIndex];

	MoveReflect(alien.body, wave, dt, world.bounds);

	const Vector2D size = alien.body.size;
	// Border check vertical:
	if (alien.body.pos.y >= world.bounds.y - size.y)
	{
		// If an alien ship reaches the bottom of the screen the players die
		world.DeletePlayers();
	}
	if (alien.body.pos.y > world.bounds.y - size.y * 0.5f)
	{
		DestroyAlien(alien, wave);
	}

	// The amount of energy increases randomly per frame
	alien.gameState.energy += gameConfig.alienUpdateRate;

	// State machine
	switch (alien.state)
	{
		case Alien::State::normal:
			if (alien.gameState.energy >= gameConfig.alienTransformEnergy)
			{
				// Transform into a better alien
				AlienTransformIntoBetter(alien);
			}
			break;
		default:
			break;
	};

	if (alien.gameState.fireTimer == 0.f)
	{ 
		// Randomly shoot lasers
		AlienSetFireTimer(alien, gameConfig, world.rndFloat01, world.rGen);
	}
	alien.gameState.fireTimer -= dt;
	if (alien.gameState.fireTimer < 0.f)
	{
		if (AlienCanShoot(alien, wave)) {
			const Vector2D laserPos = { alien.body.pos.x, alien.body.pos.y + size.y * 0.5f }; // spawn in front
			world.SpawnAlienLaser( NewLaser(laserPos, { 0.f, gameConfig.alienLaserVelocity }, { GetImageId(GameImageId::alienLaser), Color::greenIntense }, -1, ColliderId::alienLaser) );
			alien.gameState.fireTimer = 0.f; // reset it
		}
	}
}


using AIScript = void (*)(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig);
const AIScript aiScripts[numAlienScripts] = 
{
	AlienScript
};


}

extern "C"
{

DLL_EXPORT void ExecuteAlienScript(AlienScriptId scriptId, Alien& alien, const ScriptArgs& args)
{
	aiScripts[(int)scriptId](alien, args.dt, *args.world, *args.gameConfig);
}

}