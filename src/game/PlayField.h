#pragma once

#include <engine/Vector2D.h>
#include <engine/Random.h>
#include <vector>
#include <random>
#include <memory>


struct PlayerShip;
struct Alien;
struct AlienWave;
struct Laser;
struct Explosion;
struct PowerUp;
struct Wall;
struct Particle;
struct GameConfig;
struct RenderItem;
class CollisionSpace;
struct AIModule;

struct Star
{
	Vector2D pos;
	float    t;
	float    speed;
};

class PlayField
{
public:

	PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen);
	~PlayField();

	const std::vector<PlayerShip>& GetPlayers() const;
	bool NoAliens() const;
	bool NoParticles() const;

	void GetRenderItems(std::vector<RenderItem>& ritems);
	const Vector2D& GetBounds() const;

	void Restart();

	void SpawnPlayerLaser(const Laser& laser);

	//
	void SpawnAlienLaser(const Laser& laser);

	void AddPlayerShip(const PlayerShip& playerShip);

	void AddAlienShip(const Alien& alienShip);

	void AddExplosion(const Vector2D& position, float timer, float delay = 0.f);

	void SpawnPowerUp(const Vector2D& position, int type);

	void SpawnRandomPowerUp(const Vector2D& position);

	void SpawnBomb();

	void AddWall(const Vector2D& position);

	void KillPlayers();

	void RemoveDead();

	void DestroyAll();

	void DestroyWalls();

	void DestroyAllLasers();

	void DestroyAllExplosions();

	void AddScore(int score, int playerId);

	std::default_random_engine& rGen;
	const GameConfig&           config;

	// Game objects
	std::vector<PlayerShip>  players;
	std::vector<Alien>       aliens;
	std::vector<AlienWave>   alienWaves;
	std::vector<Laser>       lasers;
	std::vector<Explosion>   explosions;
	std::vector<PowerUp>     powerUps;
	std::vector<Wall>        walls;
	std::vector<Particle>    particles;
	std::vector<Star>        stars;

	Vector2D                 bounds;
	std::uniform_real_distribution<float> rndFloat01;
	Random rndPowerUp;
};
