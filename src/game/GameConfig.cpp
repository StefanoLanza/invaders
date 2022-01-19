#include "GameConfig.h"

GameConfig::GameConfig()
{
	// Console
	consoleWidth = 160;
	consoleHeight = 54;
	fontSize = 12;
	// Player
	playerFireRate = 0.2f;
	playerLaserVelocity = 50.f;
	// Alien
	alienWaveSpeedInc = 2.f;
	alienWaveFireRateInc = 0.1f;
	alienLaserVelocity = 30.f;
	// Explosions
	explosionTimer = 0.25f;
	// Power ups
	powerUpHits = 10;  // destroyed enemies have a 10% chance to drop a power-up that moves towards the bottom of the screen).
	powerUpVelocity = 8.f;
	powerUpShieldTime = 10.f;
	powerUpFireBoost = 1.5f;
	alienDestroyedScore = 10;
	bossDestroyedScore = 100;
	// Walls
	wallMaxHits = 2;

	godMode = false;  // good for testing
	randomSeed = 1734176512;
}
