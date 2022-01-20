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
#include "GameData.h"
#include "AIModule.h"

#include <engine/Timeline.h>
#include <engine/Input.h>
#include <engine/MessageLog.h>
#include <engine/Console.h>
#include <engine/CollisionSpace.h>

#include <cassert>
#include <functional>

extern const wchar_t consoleSymbols[];

namespace
{


struct PlayGameStateData
{
	MessageLog* messageLog;
	CollisionSpace collisionSpace;
	int            stageIndex = -1;
	bool           showStage;
	bool           showScore;
	int            numHits;
	bool           canPlay;
};
PlayGameStateData playGameStateData;
Timeline timeline;

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
void Collision_AlienVSLaser(void* ctx, void* ud0, void* ud1);
void Collision_PlayerVSLaser(void* ctx, void* ud0, void* ud1);
void Collision_PlayerVSPowerUp(void* ctx, void* ud0, void* ud1);
void Collision_PlayerVSAlien(void* ctx, void* ud0, void* ud1);
void Collision_LaserVSWall(void* ctx, void* ud0, void* ud1);
void ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig);
void SetLevel(int levelIndex, PlayGameStateData& data, PlayField& world);
void RestartGame(PlayGameStateData& stateData, Game& game);
void ProcessEvent(const Event& event, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig);
void SpawnAlienWave(const AlienWaveInfo& waveInfo, PlayField& world, const GameConfig& config);
void Start(Game& game, const GameConfig& config, Game::Mode mode);
void CreatePlayers(Game& game, PlayField& world, Game::Mode mode);
void DisplayLivesAndScores(const Game& game, Console& console);


void CreateStars(PlayField& world, size_t count)
{
	world.stars.resize(count);
	float lastX = -1;
	for (Star& star : world.stars) 
	{
		float x = 0.f;
		do
		{
			x = 8.f + world.rndFloat01(world.rGen) * (world.bounds.x - 16.f);
		} while (std::abs(x - lastX) < 2.f);
		lastX = x;
		const float rand = world.rndFloat01(world.rGen);
		float y = rand * world.bounds.y;
		star.pos = {x,y};
		star.t = rand;
		star.speed = rand > 0.5f ? 6.f : 3.f;
	}
}

void AnimateStars(PlayField& world, float dt)
{
	for (Star& star : world.stars)
	{
		star.pos.y += star.speed * dt;
		star.t += dt;
		if (star.t > 1.f) 
		{ 
			star.t -= 1.f;
		}
		// Wrap
		if (star.pos.y > world.bounds.y)
		{
			star.pos.y -= world.bounds.y;
		}
	}
}

}


void EnterPlayGame(void* data, Game& game, int currentState)
{
	timeline.SetCallback([&game](const Event& event) { 
		ProcessEvent(event, game.messageLog, game.world, game.config); });

	playGameStateData.messageLog = &game.messageLog;
	if (currentState != (int)GameStateId::paused)
	{
		RestartGame(playGameStateData, game);
		CreateStars(game.world, 30);
	}
}

// TODO
/*
void ExitPlayGame() {
	world.DestorYall();
}
*/


int PlayGame(Game& game, void* data, float dt)
{
	PlayField& world = game.world;
	if (world.GetPlayers().empty())
	{
		game.messageLog.Clear();
		return (int)GameStateId::over;
	}

	AnimateStars(game.world, dt);

	// Update score and lives
	for (const auto& player : world.GetPlayers())
	{
		assert(player.id < game.maxPlayers);
		game.playerScore[player.id] = player.score;
		game.playerLives[player.id] = player.lives;
	}

	if (timeline.IsOver() && world.NoAliens() && world.NoParticles())
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
	if (! playGameStateData.showStage)
	{
		TickPlayers(world, dt);
	}
	UpdateWorld(world, dt, game.scriptModule, playGameStateData.collisionSpace);
	world.RemoveDead();
	CheckCollisions(world, playGameStateData.collisionSpace, playGameStateData);

	return (int)GameStateId::play;
}


void DisplayPlayGame(Console& console, const void* data)
{
	const Game& game = *static_cast<const Game*>(data);
	if (playGameStateData.showStage && 0) //FIXME
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


namespace
{

void SpawnParticles(PlayField& world, const Vector2D& pos, int particleCount, float velocity, int life, Color color, float randomOffset)
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
		p.color = color;
		world.particles.push_back(p);
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
}

void CheckCollisions(PlayField& world, CollisionSpace& collisionSpace, PlayGameStateData& stateData)
{
	// Collision detection
	collisionSpace.Clear();
	for (auto& player : world.players)
	{
		collisionSpace.Add(GetCollisionArea(player));
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
		collisionSpace.Add(GetCollisionArea(wall));
	}

	static const CollisionCallbackInfo callbacks[] = 
	{
		{ ColliderId::player, ColliderId::alienLaser, Collision_PlayerVSLaser },
		{ ColliderId::player, ColliderId::powerUp, Collision_PlayerVSPowerUp },
		{ ColliderId::alien, ColliderId::playerLaser, Collision_AlienVSLaser },
		{ ColliderId::playerLaser, ColliderId::alienLaser, Collision_LaserVSLaser },
		{ ColliderId::playerLaser, ColliderId::wall, Collision_LaserVSWall },
		{ ColliderId::player, ColliderId::alien, Collision_PlayerVSAlien },
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
	SpawnParticles(*context.world, pos, 7, 32.f, 30, Color::yellowIntense, 0.5f);
	DestroyLaser(alienLaser);
	DestroyLaser(playerLaser);
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
		wave.speed += gameConfig.alienWaveSpeedInc;
		wave.fireRate += gameConfig.alienWaveFireRateInc;
		AlienDestroy(alien);
		context.world->AddScore( gameConfig.alienDestroyedScore, playerLaser.ownerId);
		particleCount = 7;
		particleColor = Color::yellowIntense;
	}
	SpawnParticles(*context.world, alien.body.pos, particleCount, 32.f, 30, particleColor, alien.randomOffset);

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
	SpawnParticles(*context.world, explosionPos, 7, 32.f, 30, Color::yellowIntense, 0.f);
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
	SpawnParticles(*context.world, alien.body.pos, 7, 32.f, 30, Color::yellowIntense, alien.randomOffset);
}


void Collision_PlayerVSPowerUp(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	PowerUp& powerUp = *(PowerUp*)ud1;
	ActivatePowerUp(player, powerUp, *context.messageLog, *context.world, *context.gameConfig);
	PowerUpDestroy(powerUp);
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
	world.DestroyAllExplosions();
	data.stageIndex = levelIndex;
	data.showStage = true;
	data.showScore = false;
	data.numHits = 0;
	data.canPlay = false;
	timeline.SetEvents(GetStage(levelIndex).events, GetStage(levelIndex).numEvents);
}


void RestartGame(PlayGameStateData& stateData, Game& game)
{
	Start(game, game.config, game.mode);
	SetLevel(0, stateData, game.world);
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
			messageLog.AddMessage((const char*)event.data, Color::yellowIntense);
			break;
		case GameEventId::spawnWave:
			SpawnAlienWave(*(const AlienWaveInfo*)event.data, world, gameConfig);
			break;
		case GameEventId::wait:
			break;
		default:
			break;
	};
}


void SpawnAlienWave(const AlienWaveInfo& waveInfo, PlayField& world, const GameConfig& config)
{
	const int waveIndex = (int)world.alienWaves.size();
	AlienWave wave {};
	wave.numAliens = 0;
	wave.speed = 0.f;
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
	std::unique_ptr<Input> input0, input1;

	const PlayerPrefab& prefab0 = GetPlayerPrefab(0);
	const PlayerPrefab& prefab1 = GetPlayerPrefab(1);
	const Vector2D player0Size = GetImageSize(prefab0.imageId);
	const Vector2D player1Size = GetImageSize(prefab1.imageId);

	if (mode == Game::Mode::p1 || mode == Game::Mode::p1p2)
	{
		input0 = std::make_unique<KeyboardInput>(KeyCode::left, KeyCode::right, KeyCode::rctrl);
	}
	else
	{
		input0 = std::make_unique<RndInput>(game.rGen);
	}
	if (mode == Game::Mode::p1p2)
	{
		input1 = std::make_unique<KeyboardInput>(KeyCode::A, KeyCode::D, KeyCode::lctrl);
	}
	if (input1)
	{
		world.AddPlayerShip( NewPlayerShip( { worldBounds.x / 2 - player0Size.x, worldBounds.y - player0Size.y * 0.45f }, prefab0, 0, std::move(input0) ) );
		world.AddPlayerShip( NewPlayerShip( { worldBounds.x / 2 + player1Size.x, worldBounds.y - player1Size.y * 0.45f }, prefab1, 1, std::move(input1) ) );
		game.numPlayers = 2;
	}
	else
	{
		world.AddPlayerShip( NewPlayerShip( { worldBounds.x / 2, worldBounds.y - player0Size.y * 0.45f }, prefab0, 0, std::move(input0) ) );
		game.numPlayers = 1;
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


}
