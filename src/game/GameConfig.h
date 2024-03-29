#pragma once

// Game configuration
struct GameConfig
{
	// Console
	int   consoleWidth;
	int   consoleHeight;
	int   fontSize;
	// Player
	float playerFireRate;
	float playerLaserVelocity;
	// Alien
	float alienWaveSpeedInc;
	float alienWaveFireRateInc;
	float alienLaserVelocity;
	int powerUpHits;  // destroyed enemies have a 10% chance to drop a power-up that moves towards the bottom of the screen).
	// Explosions
	float explosionTimer;  // explosion lasts some time  before it disappears
	// Power ups
	float powerUpVelocity;
	float powerUpShieldTime;
	float powerUpFireBoost;
	int  alienDestroyedScore;
	int bossDestroyedScore;
	// Walls
	int wallMaxHits;
	// Simulation
	bool godMode;  // good for testing
	unsigned int randomSeed; // > 0 to specify a fixed random seed for testing

	GameConfig();
};
