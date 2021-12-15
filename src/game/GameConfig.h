#pragma once

// Game configuration
struct GameConfig
{
	// Console
	int   fontSize;
	// World
	int   worldWidth;
	int   worldHeight;
	int   maxAlienLasers;
	int   maxPlayerLasers;
	// Player
	float playerFireRate;
	// Alien
	float alienSpeedMul;
	float alienUpdateRate;
	float alienTransformEnergy;
	float alienFireRate;  // #lasers / s
	float alienDownVelocity;
	float betterAlienFireRate; // #lasers / s
	int powerUpHits;  // destroyed enemies have a 10% chance to drop a power-up that moves towards the bottom of the screen).
	// Lasers
	float playerLaserVelocity;
	float alienLaserVelocity;
	// Explosions
	float explosionTimer;  // explosion lasts some time  before it disappears
	// Power ups
	float powerUpVelocity;
	float powerUpInvulnerabilityTime;
	float powerUpFireBoost;
	// Walls
	int wallMaxHits;
	// Simulation
	bool godMode;  // good for testing
	unsigned int randomSeed; // > 0 to specify a fixed random seed for testing

	GameConfig();
};
