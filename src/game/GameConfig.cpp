#include "GameConfig.h"


// Game configuration
GameConfig::GameConfig()
{
	// Console
	consoleWidth = 160;
	consoleHeight = 54;
	fontSize = 12;
	// World
	maxAlienLasers = 10;
	maxPlayerLasers = 4;
	// Player
	playerFireRate = 0.3f;
	// Alien
	alienWaveSpeedInc = 2.f;
	alienWaveFireRateInc = 0.1f;
	alienUpdateRate = 0.003f;
	alienTransformEnergy = 1.f;
	alienDownVelocity = 0.5f;
	// Bosses
	bossFireRate = 1.f; 
	// Explosions
	explosionTimer = 0.25f;
	// Power ups
	powerUpHits = 10;  // destroyed enemies have a 10% chance to drop a power-up that moves towards the bottom of the screen).
	powerUpVelocity = 8.f;
	powerUpInvulnerabilityTime = 10.f;
	powerUpFireBoost = 1.5f;
	alienDestroyedScore = 10;
	// Lasers
	alienLaserVelocity = 30.f;
	playerLaserVelocity = 50.f;
	// Walls
	wallMaxHits = 2;

	godMode = false;  // good for testing
	randomSeed = 1734176512;
}
