#include "stdafx.h"
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
	playerVelocity = 25.f;
	playerFireRate = 0.3f;
	// Alien
	alienUpdateRate = 0.01f;
	alienTransformEnergy = 1.f;
	alienTransformRate = 0.005f;
	alienHealth = 1.f;
	alienVelocity = 10.f;
	alienDownVelocity = 0.5f;
	alienFireRate = 1.0f;
	betterAlienAcceleration = 2.f;
	betterAlienFireRate = 0.75f;
	// Explosions
	explosionTimer = 0.25f;
	// Power ups
	powerUpRate = 0.1f;  // destroyed enemies have a 10% chance to drop a power-up that moves towards the bottom of the screen).
	powerUpVelocity = 8.f;
	powerUpInvulnerabilityTime = 10.f;
	// Lasers
	alienLaserVelocity = 20.f;
	playerLaserVelocity = 40.f;
	// Walls
	wallMaxHits = 2;

	godMode = false;  // good for testing
	randomSeed = 0; // use random_device
}
