#include "GameConfig.h"


// Game configuration
GameConfig::GameConfig()
{
	// Console
	fontSize = 12;
	// World
	worldWidth = 160;
	worldHeight = 54;
	maxAlienLasers = 10;
	maxPlayerLasers = 4;
	// Player
	playerFireRate = 0.3f;
	// Alien
	alienSpeedMul = 1.f;
	alienUpdateRate = 0.003f;
	alienTransformEnergy = 1.f;
	alienDownVelocity = 0.5f;
	alienFireRate = 1.0f;
	betterAlienFireRate = 0.75f;
	// Explosions
	explosionTimer = 0.25f;
	// Power ups
	powerUpHits = 10;  // destroyed enemies have a 10% chance to drop a power-up that moves towards the bottom of the screen).
	powerUpVelocity = 8.f;
	powerUpInvulnerabilityTime = 10.f;
	// Lasers
	alienLaserVelocity = 30.f;
	playerLaserVelocity = 50.f;
	// Walls
	wallMaxHits = 2;

	godMode = false;  // good for testing
	randomSeed = 1734176512;
}
