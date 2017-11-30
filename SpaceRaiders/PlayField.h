#pragma once

#include "Vector2D.h"
#include "Random.h"
#include <vector>
#include <random>


class PlayerShip;
class Alien;
class PlayerLaser;
class AlienLaser;
class Explosion;
class PowerUp;
class Wall;
struct GameConfig;
struct RenderItem;
class MessageLog;


class PlayField
{
public:

	PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen);
	~PlayField();

	const std::vector<PlayerShip>& GetPlayers() const;
	bool NoAliens() const;

	void GetRenderItems(std::vector<RenderItem>& ritems);
	const Vector2D& GetBounds() const;

	void Restart();

	void SpawnPlayerLaser(const PlayerLaser& laser);

	//
	void SpawnAlienLaser(const AlienLaser& laser);

	void AddPlayerShip(const PlayerShip& playerShip);

	void AddAlienShip(const Alien& alienShip);

	void AddExplosion(const Vector2D& position, float timer);

	void SpawnPowerUp(const Vector2D& position);

	void AddWall(const Vector2D& position);

	void EndGame();

	// TODO Remove gamePlay from PlayField
	void Update(float dt, MessageLog& messageLog);

	void DestroyAll();

	void DestroyWalls();

	int GetAvailablePlayerLasers() const;

private:

	// Collision matrix
	void CheckCollisions_LasersVSLasers();
	void CheckCollisions_AliensVSLasers();
	void CheckCollisions_PlayersVSLasers();
	void CheckCollisions_PlayersVSAliens();
	void CheckCollisions_PlayersVSPowerUps(MessageLog& messageLog);
	void CheckCollisions_LasersVSWalls();
	void CheckCollisions_AliensVSWalls();

	void EnanchePlayer(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog) const;
	void AddScore(int score, int playerId);

private:

	std::default_random_engine& rGen;
	const GameConfig&           config;

	// Game objects
	std::vector<PlayerShip>  players;
	std::vector<Alien>       aliens;
	std::vector<PlayerLaser> playerLasers;
	std::vector<AlienLaser>  alienLasers;
	std::vector<Explosion>   explosions;
	std::vector<PowerUp>     powerUps;
	std::vector<Wall>        walls;

	Vector2D                 bounds;
	std::uniform_real_distribution<float> rndFloat01;
	Random rndPowerUp;
	// Number of available active laser slots for aliens and player
	int availableAlienLasers;
	int availablePlayerLasers;
};
