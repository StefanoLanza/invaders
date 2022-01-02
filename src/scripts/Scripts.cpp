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

void MoveNormalAlien(Body& body, AlienWave& wave, float dt, const Vector2D& worldBounds)
{
	const float halfWidth = body.size.x * 0.5f;
	body.prevPos = body.pos;
	body.velocity.x = wave.speed * wave.direction;
	body.pos = Mad(body.pos, body.velocity, dt);

	// Border check
	if (body.pos.x < halfWidth)
	{
		body.pos.x = halfWidth;
		//body.pos.y += body.size.y;
		wave.direction = 1;
	}
	else if (body.pos.x > worldBounds.x - halfWidth)
	{
		body.pos.x = (float)worldBounds.x - halfWidth;
		wave.direction = -1;
		//body.pos.y += body.size.y;
	}
}

void MoveSentinelAlien(Body& body, float dt, const Vector2D& worldBounds)
{
	const float halfWidth = body.size.x * 0.5f;
	body.prevPos = body.pos;
	body.pos = Mad(body.pos, body.velocity, dt);

	// Border check
	if (body.pos.x < halfWidth)
	{
		body.pos.x = halfWidth;
		//body.pos.y += body.size.y;
		body.velocity.x = std::abs(body.velocity.x);
	}
	else if (body.pos.x > worldBounds.x - halfWidth)
	{
		body.pos.x = (float)worldBounds.x - halfWidth;
		body.velocity.x = -std::abs(body.velocity.x);
		//body.pos.y += body.size.y;
	}
}

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

	MoveNormalAlien(alien.body, wave, dt, world.bounds);

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

	if (alien.gameState.fireTimer == 0.f)
	{ 
		// Randomly shoot lasers
		alien.gameState.fireTimer = (1.f / wave.fireRate) * (1.f + world.rndFloat01(world.rGen));
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


void SentinelAlienScript(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{

}

void BossAlienScript(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig)
{

}

using AIScript = void (*)(Alien& alien, float dt, PlayField& world, const GameConfig& gameConfig);
const AIScript aiScripts[numAlienScripts] = 
{
	NormalAlienScript,
	SentinelAlienScript,
	BossAlienScript,
};

}

extern "C"
{

DLL_EXPORT void ExecuteAlienScript(AlienScriptId scriptId, Alien& alien, const ScriptArgs& args)
{
	aiScripts[(int)scriptId](alien, args.dt, *args.world, *args.gameConfig);
}

DLL_EXPORT void ExecuteAlienWaveScript(AlienWave* wave, Alien* aliens, const ScriptArgs& args)
{
}

}