#include "PlayGameState.h"
#include "GameStates.h"
#include "GameStateMgr.h"
#include "Game.h"
#include "GameConfig.h"
#include "PlayField.h"
#include "Player.h"
#include "Alien.h"
#include "Laser.h"
#include "PowerUp.h"
#include "Wall.h"
#include "Particle.h"
#include "Explosion.h"
#include "Asteroid.h"
#include "GameData.h"
#include "AIModule.h"

#include <engine/Timeline.h>
#include <engine/Input.h>
#include <engine/MessageLog.h>
#include <engine/Console.h>
#include <engine/CollisionSpace.h>
#include <engine/span.h>

#include <cassert>
#include <functional>

namespace
{


struct PlayGameStateData
{
	PlayField*     world;
	MessageLog*    messageLog;
	CollisionSpace collisionSpace;
	int            stageIndex = -1;
	bool           showStage;
	bool           showScore;
	int            numHits;
	float          starSpeed = 1.f;
};
PlayGameStateData playGameStateData;
Timeline timeline;
KeyboardInput input0 { KeyCode::left, KeyCode::right, KeyCode::rctrl };
KeyboardInput input1 { KeyCode::A, KeyCode::D, KeyCode::lctrl };


struct CollisionContext
{
	PlayField*  world;
	const GameConfig* gameConfig;
	MessageLog* messageLog;
	PlayGameStateData* stateData;
};


void TickPlayers(PlayField& world, float dt);
void UpdateWorld(PlayField& world, float dt, const AIModule& aiModule, const CollisionSpace& collisionSpace);
void CheckCollisions(PlayField& world, CollisionSpace& collisionSpace, PlayGameStateData& stateData);
// Collision callbacks
void Collision_LaserVSLaser(void* ctx, void* ud0, void* ud1);
void Collision_LaserVSAsteroid(void* ctx, void* ud0, void* ud1);
void Collision_AlienVSLaser(void* ctx, void* ud0, void* ud1);
void Collision_PlayerVSLaser(void* ctx, void* ud0, void* ud1);
void Collision_PlayerVSPowerUp(void* ctx, void* ud0, void* ud1);
void Collision_PlayerVSAsteroid(void* ctx, void* ud0, void* ud1);
void Collision_PlayerVSAlien(void* ctx, void* ud0, void* ud1);
void Collision_LaserVSWall(void* ctx, void* ud0, void* ud1);
void Collision_AsteroidVSAsteroid(void* ctx, void* ud0, void* ud1);
void ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig);
void SetLevel(int levelIndex, PlayGameStateData& data, PlayField& world);
void RestartGame(PlayGameStateData& stateData, Game& game);
void ProcessEvent(const Event& event, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig);
void Start(Game& game, const GameConfig& config, Game::Mode mode);
void CreatePlayers(Game& game, PlayField& world, Game::Mode mode);
void DisplayLivesAndScores(const Game& game, Console& console);


void CreateStars(PlayField& world, size_t count)
{
	world.stars.resize(count);
	for (Star& star : world.stars) 
	{
		const float rand = world.rndFloat01(world.rGen);
		star.pos.x = world.rndFloat01(world.rGen) * world.bounds.x;
		star.pos.y = rand * world.bounds.y;
		star.speed = rand > 0.5f ? 12.f : 6.f;
		star.color = rand > 0.5f ? Color::whiteIntense : Color::white;
	}
}

void AnimateStars(span<Star> stars, float dt, float speedMul, const Vector2D& worldBounds)
{
	dt *= speedMul;
	for (Star& star : stars)
	{
		star.pos.y += star.speed * dt;
		// Wrap
		if (star.pos.y < 0.f)
		{
			star.pos.y += worldBounds.y;
		}
		else if (star.pos.y > worldBounds.y)
		{
			star.pos.y -= worldBounds.y;
		}
	}
}


void SpawnParticles(std::vector<Particle>& particles, const Vector2D& pos, int particleCount, float velocity, int life, ImageId imageId, Color color, float randomOffset)
{
	float angle = two_pi * randomOffset;
	float dangle = two_pi / (float)particleCount;
	for (int i = 0; i < particleCount; ++i)
	{
		Vector2D dir = Rotate({1.0f,0.f}, angle);
		Particle p;
		p.pos = Add(pos, dir);
		p.vel = Mul(dir, velocity);
		angle += dangle;
		dangle *= 1.02f; // randomize
		p.vel.y *= 0.5f; // aspect ratio
		velocity *= 1.1f; // randomize
		p.accel = Vector2D {0.f, 0.f };
		p.life = life;
		p.imageId = imageId;
		p.color = color;
		particles.push_back(p);
	}
}


void TickPlayers(PlayField& world, float dt)
{
	// First move all game objects
	for (auto& player : world.players)
	{
		Move(player,dt, world.bounds, world, world.config);
	}
}

void UpdateWorld(PlayField& world, float dt, const AIModule& aiModule, const CollisionSpace& collisionSpace)
{
	// Update all waves
	const ScriptArgs scriptArgs = { dt, nullptr, &world, &world.config, &collisionSpace };

	for (auto& powerUp : world.powerUps)
	{
		PowerUpMove(powerUp, dt, world.bounds);
	}
	for (auto& laser : world.lasers)
	{
		MoveLaser(laser, dt, world.bounds);
	}
	for (auto& alienShip : world.aliens)
	{
		AlienUpdate(alienShip, dt, world, world.config, world.alienWaves[alienShip.waveIndex]);
	}
	for (auto& explosion : world.explosions)
	{
		UpdateExplosion(explosion, dt);
	}
	for (auto& particle : world.particles)
	{
		// Euler integration
		particle.pos = Mad(particle.pos, particle.vel, dt);
		particle.vel = Mad(particle.vel, particle.accel, dt);
		--particle.life;
	}
	if (aiModule.alienScript)
	{
		for (auto& alien : world.aliens)
		{
			aiModule.alienScript(alien, scriptArgs);
		}
	}
	for (auto& asteroid : world.asteroids)
	{
		UpdateAsteroid(asteroid, dt, world.bounds);
	}
}

void CheckCollisions(PlayField& world, CollisionSpace& collisionSpace, PlayGameStateData& stateData)
{
	collisionSpace.Clear();
	for (auto& player : world.players)
	{
		collisionSpace.Add(GetCollider(player));
	}
	for (auto& alien : world.aliens)
	{
		collisionSpace.Add(AlienGetCollider(alien));
	}
	for (auto& laser : world.lasers)
	{
		collisionSpace.Add(GetCollider(laser));
	}
	for (auto& powerUp : world.powerUps)
	{
		collisionSpace.Add(PowerUpGetCollider(powerUp));
	}
	for (auto& wall : world.walls)
	{
		collisionSpace.Add(GetCollider(wall));
	}
	for (auto& asteroid : world.asteroids)
	{
		if (asteroid.state == Asteroid::State::falling) 
		{
			collisionSpace.Add(GetCollider(asteroid));
		}
	}

	static const CollisionCallbackInfo callbacks[] = 
	{
		{ ColliderId::player, ColliderId::alienLaser, Collision_PlayerVSLaser },
		{ ColliderId::player, ColliderId::powerUp, Collision_PlayerVSPowerUp },
		{ ColliderId::alien, ColliderId::playerLaser, Collision_AlienVSLaser },
		{ ColliderId::playerLaser, ColliderId::alienLaser, Collision_LaserVSLaser },
		{ ColliderId::playerLaser, ColliderId::wall, Collision_LaserVSWall },
		{ ColliderId::player, ColliderId::alien, Collision_PlayerVSAlien },
		{ ColliderId::player, ColliderId::asteroid, Collision_PlayerVSAsteroid },
		{ ColliderId::playerLaser, ColliderId::asteroid, Collision_LaserVSAsteroid },
		{ ColliderId::asteroid, ColliderId::asteroid, Collision_AsteroidVSAsteroid },
	};

	CollisionContext context =
	{
		&world,
		&world.config,
		stateData.messageLog,
		&stateData
	};
	collisionSpace.Execute(callbacks, (int)std::size(callbacks), &context);
}


void Collision_LaserVSLaser(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Laser& alienLaser = *static_cast<Laser*>(ud1);
	// Spawn explosion, kill this and the alien laser
	Vector2D pos = Lerp(playerLaser.body.pos, alienLaser.body.pos, 0.5f);
	SpawnParticles(context.world->particles, pos, 7, 32.f, 30, GameImageId::particle, Color::yellowIntense, 0.5f);
	DestroyLaser(alienLaser);
	DestroyLaser(playerLaser);
}


void Collision_LaserVSAsteroid(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Asteroid& asteroid = *static_cast<Asteroid*>(ud1);
	// Spawn explosion, kill this and the alien laser
	Vector2D pos = Lerp(playerLaser.body.pos, asteroid.body.pos, 0.5f);
	DestroyLaser(playerLaser);
	HitAsteroid(asteroid);
	const ImageId imageId = (asteroid.state == Asteroid::State::dead) ? GameImageId::asteroidParticle : GameImageId::particle;
	const Color color = (asteroid.state == Asteroid::State::dead) ? asteroid.visual.color : Color::yellowIntense;
	const float randomOffset = (asteroid.state == Asteroid::State::dead) ? 0.5f : 0.f;
	SpawnParticles(context.world->particles, asteroid.body.pos, 7, 32.f, 30, imageId, color, randomOffset);
}

void Collision_AsteroidVSAsteroid(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	Asteroid& asteroid0 = *static_cast<Asteroid*>(ud0);
	Asteroid& asteroid1 = *static_cast<Asteroid*>(ud1);
	Vector2D pos = Lerp(asteroid0.body.pos, asteroid1.body.pos, 0.5f);
	DestroyAsteroid(asteroid0);
	DestroyAsteroid(asteroid1);
	SpawnParticles(context.world->particles, pos, 7, 32.f, 30, GameImageId::asteroidParticle, asteroid0.visual.color, 0.f);
}

void Collision_AlienVSLaser(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	const GameConfig& gameConfig = *context.gameConfig;
	Alien& alien = *(Alien*)ud0;
	Laser& playerLaser = *(Laser*)ud1;
	DestroyLaser(playerLaser);
	int particleCount = 3;
	Color particleColor = Color::redIntense;
	if (AlienHit(alien)) {
		AlienWave& wave = context.world->alienWaves[alien.waveIndex];
		//wave.speed += gameConfig.alienWaveSpeedInc;
		AlienDestroy(alien);
		context.world->AddScore( gameConfig.alienDestroyedScore, playerLaser.ownerId);
		particleCount = 7;
		particleColor = Color::yellowIntense;
	}
	SpawnParticles(context.world->particles, alien.body.pos, particleCount, 32.f, 30, GameImageId::particle, particleColor, alien.randomOffset);

	// Spawn random power ups
	++context.stateData->numHits;
	if (context.stateData->numHits >= gameConfig.powerUpHits)
	{
		context.world->SpawnRandomPowerUp(alien.body.pos);
		context.stateData->numHits =  0;
	}
}


void Collision_PlayerVSLaser(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	Laser& alienLaser = *(Laser*)ud1;
	// Spawn explosion, destroy player and laser
	DestroyLaser(alienLaser);
	if (! context.gameConfig->godMode && ! player.hasShield)
	{
		PlayerHit(player);
	}
	Vector2D explosionPos = player.body.pos;
	if (player.hasShield)
	{
		explosionPos = Add(explosionPos, { 0.f, -3.f} );
	}
	SpawnParticles(context.world->particles, explosionPos, 7, 32.f, 30, GameImageId::particle, Color::yellowIntense, 0.f);
}


void Collision_PlayerVSAlien(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	Alien& alien = *(Alien*)ud1;
	//Spawn explosion, destroy player and alien
	AlienDestroy(alien);
	if (! context.gameConfig->godMode)
	{
		PlayerHit(player);
	}
	SpawnParticles(context.world->particles, alien.body.pos, 7, 32.f, 30, GameImageId::particle, Color::yellowIntense, alien.randomOffset);
}


void Collision_PlayerVSPowerUp(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	PowerUp& powerUp = *(PowerUp*)ud1;
	ActivatePowerUp(player, powerUp, *context.messageLog, *context.world, *context.gameConfig);
	PowerUpDestroy(powerUp);
}


void Collision_PlayerVSAsteroid(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	Asteroid& asteroid = *(Asteroid*)ud1;
	PlayerHit(player);
	DestroyAsteroid(asteroid);
	SpawnParticles(context.world->particles, asteroid.body.pos, 7, 32.f, 30, GameImageId::asteroidParticle, asteroid.visual.color, 0.f);
}


void Collision_LaserVSWall(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	DestroyLaser(playerLaser);
	HitWall(wall);
	const Vector2D pos = { wall.pos.x,  wall.pos.y + 1.f };
	context.world->AddExplosion(pos, context.gameConfig->explosionTimer);
}


void ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig)
{
	switch (powerUp.type)
	{
	case PowerUp::Type::speedBoost:
		messageLog.AddMessage("Speed Boost!");
		player.SetSpeedBoost(2);
		break;
	case PowerUp::Type::fireBoost:
		messageLog.AddMessage("Fire Boost!");
		player.SetFireBoost(gameConfig.powerUpFireBoost);
		break;
	case PowerUp::Type::doubleFire:
		messageLog.AddMessage("Double Fire!");
		player.SetDoubleFire();
		break;
	case PowerUp::Type::tripleFire:
		messageLog.AddMessage("Triple Fire!");
		player.SetTripleFire();
		break;
	case PowerUp::Type::shield:
		messageLog.AddMessage("Shield!");
		player.SetShield(gameConfig.powerUpShieldTime);
		break;
	case PowerUp::Type::bomb:
		messageLog.AddMessage("Bomb!");
		world.SpawnBomb();
		break;
	default:
		break;
	}
}


void SetLevel(int levelIndex, PlayGameStateData& data, PlayField& world)
{
	world.DestroyAllLasers();
	data.stageIndex = levelIndex;
	data.showStage = true;
	data.showScore = false;
	data.numHits = 0;
	timeline.SetEvents(GetStage(levelIndex).events, GetStage(levelIndex).numEvents);
}


void RestartGame(PlayGameStateData& stateData, Game& game)
{
	Start(game, game.config, game.mode);
	SetLevel(0, stateData, game.world);
}


void SpawnAlienWave(const AlienWaveInfo& waveInfo, PlayField& world, const GameConfig& config)
{
	const int waveIndex = (int)world.alienWaves.size();
	AlienWave wave {};
	wave.numAliens = 0;
	wave.speedRate = 100;
	wave.fireRate = 0.f;
	wave.numReadyAliens = 0;

	const float x0 = (world.bounds.x - (waveInfo.numCols * waveInfo.dx)) / 2.f + (waveInfo.dx / 2);
	float y = waveInfo.start_y;
	const char* c = waveInfo.mask;
	for (int j = 0, alienIndex = 0; j < waveInfo.numRows; ++j, y += waveInfo.dy)
	{
		float x = x0;
		for (int k = 0; k < waveInfo.numCols; ++k, x += waveInfo.dx)
		{
			if (const int prefabId = *c - '0'; prefabId >= 0 && prefabId < GetNumAlienPrefabs())
			{
				const Path& enterPath = GetEnterPath(waveInfo.enterPath[alienIndex]);
				const Path& attackPath = GetAttackPath(waveInfo.attackPath[alienIndex]);
				const AlienPrefab& alienPrefab = GetAlienPrefab(prefabId);
				Vector2D gridPos { x, y };
				Alien alien = NewAlien(gridPos, alienPrefab, world.rndFloat01(world.rGen),
					enterPath, waveInfo.enterDelay[alienIndex],
					attackPath);
				alien.waveIndex = waveIndex;
				world.AddAlienShip(alien);
				++wave.numAliens;
			}
			++c;
			++alienIndex;
		}
	}

	wave.numAliveAliens = wave.numAliens;
	world.alienWaves.push_back(wave);
}


void SpawnAsteroids(const AsteroidShowerDef& def, PlayField& world, const GameConfig& config)
{
	world.asteroids.reserve(world.asteroids.size() + def.count);
	int enterDelay = 0;
	std::uniform_int_distribution<int> rndInt(0,GetNumAsteroidDefs() - 1);
	for (int k = 0; k < def.count; ++k)
	{
		int a = rndInt(world.rGen);
		float r01 = world.rndFloat01(world.rGen);
		float s01 = r01 < 0.5f ? (0.5f + r01) : (r01 - 0.5f);
		float x =  r01 * world.bounds.x;
		float sx = std::lerp(def.minSpeed_x, def.maxSpeed_x, s01);
		float sy = std::lerp(def.minSpeed_y, def.maxSpeed_y, world.rndFloat01(world.rGen));
		const AsteroidDef& adef = GetAsteroidDef(a);
		Vector2D pos { x, def.start_y };
		Asteroid asteroid = NewAsteroid(pos, { sx, sy }, adef.imageId, enterDelay);
		world.asteroids.push_back(asteroid);
		enterDelay += def.delay;
	}
}


void ProcessEvent(const Event& event, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig)
{
	switch (event.id)
	{
		case GameEventId::showStage:
			playGameStateData.showStage = true;
			break;
		case GameEventId::hideStage:
			playGameStateData.showStage = false;
			break;
		case GameEventId::showScore:
			playGameStateData.showScore = true;
			break;
		case GameEventId::hideScore:
			playGameStateData.showScore = false;
			break;
		case GameEventId::message:
			messageLog.AddMessage(event.data.str, Color::yellowIntense);
			break;
		case GameEventId::spawnAlienWave:
			SpawnAlienWave(*(const AlienWaveInfo*)event.data.data, world, gameConfig);
			break;
		case GameEventId::spawnAsteroids:
			SpawnAsteroids(*(const AsteroidShowerDef*)event.data.data, world, gameConfig);
			break;
		case GameEventId::wait:
			break;
		case GameEventId::setStarsSpeed:
			playGameStateData.starSpeed = event.data.fvalue;
			break;
		default:
			break;
	};
}


void Start(Game& game, const GameConfig& config, Game::Mode mode)
{
	if (config.randomSeed)
	{
		game.rGen.seed(config.randomSeed);
	}
	else
	{
		std::random_device random_device;
		game.rGen.seed( random_device() );
	}
	game.world.DestroyAll();
	for (int s = 0; s < game.maxPlayers; ++s)
	{
		game.playerScore[s] = 0;
		game.playerLives[s] = 0;
	}
	CreatePlayers(game, game.world, mode);
	game.world.Restart();
	game.messageLog.Clear();
}


void CreatePlayers(Game& game, PlayField& world, Game::Mode mode)
{
	const Vector2D worldBounds = game.world.GetBounds();
	const PlayerPrefab& prefab0 = GetPlayerPrefab(0);
	const PlayerPrefab& prefab1 = GetPlayerPrefab(1);
	const Vector2D player0Size = GetImageSize(prefab0.imageId);
	const Vector2D player1Size = GetImageSize(prefab1.imageId);

	if (mode == Game::Mode::p1)
	{
		world.AddPlayerShip( NewPlayerShip( { worldBounds.x / 2, worldBounds.y - player0Size.y * 0.45f }, prefab0, 0, &input0) );
		game.numPlayers = 1;
	}
	else
	{
		world.AddPlayerShip( NewPlayerShip( { worldBounds.x / 2 - player0Size.x, worldBounds.y - player0Size.y * 0.45f }, prefab0, 0, &input0) );
		world.AddPlayerShip( NewPlayerShip( { worldBounds.x / 2 + player1Size.x, worldBounds.y - player1Size.y * 0.45f }, prefab1, 1, &input1) );
		game.numPlayers = 2;
	}
}

void DisplayLivesAndScores(const Game& game, Console& console)
{
	const int x[2] = { 0, console.GetBounds().x / 2 };
	const Color color[2] = { Color::white, Color::lightBlueIntense };
#if 0
	for (int p = 0; p < game.numPlayers; ++p)
	{
		console.DrawImage(GetImage(GameImageId::score), x[p] + 2, 0, color[p], ImageAlignment::left, ImageAlignment::top);
		console.DrawImage(GetImage(GameImageId::lives), x[p] + 48, 0, color[p], ImageAlignment::left, ImageAlignment::top);
		console.DrawNumber(game.playerScore[p], x[p] + 40, 0, &GetImage(GameImageId::_0), color[p], TextAlignment::right);
		console.DrawNumber(game.playerLives[p], x[p] + 74, 0, &GetImage(GameImageId::_0), color[p], TextAlignment::right);
	}
#else
	for (int p = 0; p < game.numPlayers; ++p)
	{
		char tmp[256];
		snprintf(tmp, sizeof tmp, "SCORE: %d", game.playerScore[p]);
		console.DisplayText(tmp, x[p] + 2, 1, color[p]);
		snprintf(tmp, sizeof tmp, "LIVES: %d", game.playerLives[p]);
		console.DisplayText(tmp, x[p] + 16, 1, color[p]);
	}
#endif
}


void EnterPlayGame(void* data, Game& game, int currentState)
{
	playGameStateData.world = &game.world;
	playGameStateData.messageLog = &game.messageLog;

	timeline.SetCallback([&game](const Event& event) { 
		ProcessEvent(event, game.messageLog, game.world, game.config); });

	if (currentState != (int)GameStateId::paused)
	{
		RestartGame(playGameStateData, game);
		CreateStars(game.world, 30);
	}
}

void ExitPlayGame(void* data, int newState) 
{
	if (newState != (int)GameStateId::paused)
	{
		playGameStateData.world->DestroyAll();
	}
}


int PlayGame(Game& game, void* data, float dt)
{
	PlayField& world = game.world;
	if (world.GetPlayers().empty())
	{
		game.messageLog.Clear();
		return (int)GameStateId::over;
	}

	AnimateStars({world.stars.data(), world.stars.size()}, dt, playGameStateData.starSpeed, world.bounds);

	// Update score and lives
	for (const auto& player : world.GetPlayers())
	{
		assert(player.id < game.maxPlayers);
		game.playerScore[player.id] = player.score;
		game.playerLives[player.id] = player.lives;
	}

	if (timeline.IsOver() && world.aliens.empty() && world.particles.empty() && world.asteroids.empty())
	{
		if (playGameStateData.stageIndex < GetNumStages() - 1)
		{
			// Next level
			SetLevel(playGameStateData.stageIndex + 1, playGameStateData, world);
			return (int)GameStateId::play;
		}
		else
		{
			return (int)GameStateId::victory;
		}
	}
	timeline.Advance(dt);

	if (KeyJustPressed(KeyCode::escape))
	{
		return (int)GameStateId::paused;
	}
	TickPlayers(world, dt);
	UpdateWorld(world, dt, game.scriptModule, playGameStateData.collisionSpace);
	world.RemoveDead();
	CheckCollisions(world, playGameStateData.collisionSpace, playGameStateData);

	return (int)GameStateId::play;
}


void DisplayPlayGame(Console& console, const void* data)
{
	const Game& game = *static_cast<const Game*>(data);
	if (playGameStateData.showStage)
	{
		const IVector2D& bounds = console.GetBounds();
		const int boxWidth = 40;
		const int boxHeight = 10;
		const int top = (bounds.y - boxHeight) / 2; // centered
		const int left = (bounds.x - boxWidth) / 2;
		console.DrawRectangle(left, top, boxWidth, boxHeight, Color::black);
		ImageId imageId = (playGameStateData.stageIndex + (int)GameImageId::_1);
		console.DrawImage(GetImage(imageId), 0, 2, Color::green, ImageAlignment::centered, ImageAlignment::centered);
		console.DrawImage(GetImage(GameImageId::stage), 0, -3, Color::green, ImageAlignment::centered, ImageAlignment::centered);
	}
	if (playGameStateData.showScore)
	{
		DisplayLivesAndScores(game, console);
	}
}


}


void RegisterPlayState(Game& game)
{
	RegisterGameState(game, &game, PlayGame, DisplayPlayGame, EnterPlayGame, ExitPlayGame);
}
