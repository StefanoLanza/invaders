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
#include "GameData.h"

#include <engine/Timeline.h>
#include <engine/Input.h>
#include <engine/MessageLog.h>
#include <engine/Console.h>
#include <engine/CollisionSpace.h>

#include <cassert>
#include <functional>


namespace
{

struct PlayGameStateData
{
	CollisionSpace collisionSpace;
	int            levelIndex = -1;
	bool           showLevel;
	int            numHits;
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
void SpawnBoss(const BossInfo& boss, PlayField& world, const GameConfig& config);
void Start(Game& game, const GameConfig& config, Game::Mode mode);
void CreatePlayers(Game& game, PlayField& world, Game::Mode mode);

}


void EnterPlayGame(void* data, Game& game, int currentState)
{
	timeline.SetCallback([&game](const Event& event) { 
		ProcessEvent(event, game.messageLog, game.world, game.config); });

	if (currentState != (int)GameStateId::paused)
	{
		RestartGame(playGameStateData, game);
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

	// Update score
	for (const auto& player : world.GetPlayers())
	{
		assert(player.id < game.maxPlayers);
		game.score[player.id] = player.score;
	}

	if (timeline.IsOver() && world.NoAliens())
	{
		if (playGameStateData.levelIndex < GetNumLevels() - 1)
		{
			// Next level
			SetLevel(playGameStateData.levelIndex + 1, playGameStateData, world);
			return (int)GameStateId::running;
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
	if (! playGameStateData.showLevel)
	{
		world.Update(dt, game.scriptModule);
		CheckCollisions(world, playGameStateData.collisionSpace, playGameStateData);
	}

	return (int)GameStateId::running;
}


void DisplayPlayGame(Console& renderer, const void* data)
{
	if (playGameStateData.showLevel)
	{
		ImageId imageId = (playGameStateData.levelIndex + (int)GameImageId::_1);
		renderer.DrawImage(GetImage(imageId), 0, 2, Color::yellowIntense, ImageAlignment::centered, ImageAlignment::centered);
		renderer.DrawImage(GetImage(GameImageId::level), 0, -4, Color::yellowIntense, ImageAlignment::centered, ImageAlignment::centered);
	}
}


namespace
{

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
	for (auto& playerLaser : world.lasers)
	{
		collisionSpace.Add(GetCollider(playerLaser));
	}
	for (auto& powerUp : world.powerUps)
	{
		collisionSpace.Add(GetCollisionArea(powerUp));
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
		&world.messageLog,
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
	context.world->AddExplosion(pos, context.gameConfig->explosionTimer);
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
	if (AlienHit(alien))
	{
		if (alien.waveIndex >= 0) {
			AlienWave& wave = context.world->alienWaves[alien.waveIndex];
			wave.speed += gameConfig.alienWaveSpeedInc;
			wave.fireRate += gameConfig.alienWaveFireRateInc;
			AlienDestroy(alien, wave);
			context.world->AddScore( gameConfig.alienDestroyedScore, playerLaser.ownerId);
		}
		else {
			BossDestroy(alien); // FIXME separate class for bosses ?
			context.world->AddScore( gameConfig.alienDestroyedScore, playerLaser.ownerId);
		}
	}

	// Spawn explosion 
	context.world->AddExplosion(alien.body.pos, gameConfig.explosionTimer);
	if (alien.state == Alien::State::dead)  // FIXME hits or destroyed aliens?
	{
		++context.stateData->numHits;
		if (context.stateData->numHits >= gameConfig.powerUpHits)
		{
			context.world->SpawnRandomPowerUp(alien.body.pos);
			context.stateData->numHits =  0;
		}
	}
}


// FIXME Collision with shield actually
void Collision_PlayerVSLaser(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	Laser& alienLaser = *(Laser*)ud1;
	//Spawn explosion, destroy player and laser
	DestroyLaser(alienLaser);
	if (! context.gameConfig->godMode && ! player.hasShield)
	{
		player.Destroy();
	}
	Vector2D explosionPos = player.pos;
	if (player.hasShield)
	{
		explosionPos = Add(explosionPos, { 0.f, -3.f} );
	}
	context.world->AddExplosion(explosionPos, context.gameConfig->explosionTimer);
}


void Collision_PlayerVSAlien(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	Alien& alien = *(Alien*)ud1;
	//Spawn explosion, destroy player and alien
	AlienDestroy(alien, context.world->alienWaves[alien.waveIndex]);
	if (! context.gameConfig->godMode)
	{
		player.Destroy();
	}
	context.world->AddExplosion(alien.body.pos, context.gameConfig->explosionTimer);
}


void Collision_PlayerVSPowerUp(void* ctx, void* ud0, void* ud1)
{
	const CollisionContext& context = *static_cast<CollisionContext*>(ctx);
	PlayerShip& player = *(PlayerShip*)ud0;
	PowerUp& powerUp = *(PowerUp*)ud1;
	ActivatePowerUp(player, powerUp, *context.messageLog, *context.world, *context.gameConfig);
	Destroy(powerUp);
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
	case PowerUp::speedBoost:
		messageLog.AddMessage("Speed Boost!");
		player.SetSpeedBoost(2);
		break;
	case PowerUp::fireBoost:
		messageLog.AddMessage("Fire Boost!");
		player.SetFireBoost(gameConfig.powerUpFireBoost);
		break;
	case PowerUp::doubleFire:
		messageLog.AddMessage("Double Fire!");
		player.SetDoubleFire();
		break;
	case PowerUp::tripleFire:
		messageLog.AddMessage("Triple Fire!");
		player.SetTripleFire();
		break;
	case PowerUp::invulnerability:
		messageLog.AddMessage("Invulnerability!");
		player.SetInvulnerable(gameConfig.powerUpInvulnerabilityTime);
		break;
	case PowerUp::bomb:
		messageLog.AddMessage("Bomb!");
		world.SpawnBomb();
		break;
	case PowerUp::shield:
		messageLog.AddMessage("Shield!");
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
	data.levelIndex = levelIndex;
	data.showLevel = true;
	data.numHits = 0;
	timeline.SetEvents(GetLevel(levelIndex).events, GetLevel(levelIndex).numEvents);
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
			playGameStateData.showLevel = true;
			break;
		case GameEventId::hideStage:
			playGameStateData.showLevel = false;
			break;
		case GameEventId::message:
			messageLog.AddMessage((const char*)event.data, Color::yellowIntense);
			break;
		case GameEventId::spawnWave:
			SpawnAlienWave(*(const AlienWaveInfo*)event.data, world, gameConfig);
			break;
		case GameEventId::boss:
			SpawnBoss(*(const BossInfo*)event.data, world, gameConfig);
			break;
		case GameEventId::wait:
			break;
		default:
			break;
	};
}


void SpawnBoss(const BossInfo& bossInfo, PlayField& world, const GameConfig& config)
{
	const AlienPrefab& prefab = GetBossPrefab(bossInfo.alienType);
	Alien boss = NewAlien( { bossInfo.x, bossInfo.y }, { bossInfo.speed, config.alienDownVelocity }, prefab, 0.f );
	world.AddAlienShip(boss); 
}


void SpawnAlienWave(const AlienWaveInfo& waveInfo, PlayField& world, const GameConfig& config)
{
	// Look for empty wave
	int waveIndex = 0;
	for (; waveIndex < (int)world.alienWaves.size(); ++waveIndex) {
		if (world.alienWaves[waveIndex].numAliens == 0) {
			break;
		}
	}
	if (waveIndex == world.alienWaves.size()) {
		world.alienWaves.resize(waveIndex + 1);
	}
	AlienWave& wave = world.alienWaves[waveIndex];
	wave.numAliens = 0;
	wave.numCols = waveInfo.squad->numCols;
	wave.numRows = waveInfo.squad->numRows;
	wave.speed = waveInfo.initialSpeed;
	wave.direction = waveInfo.initialDirection;
	wave.fireRate = waveInfo.initialFireRate;
	wave.bound = waveInfo.bound;

	const AlienSquad& squad = *waveInfo.squad;
	float x0 = (world.GetBounds().x - (squad.numCols * waveInfo.dx)) / 2.f;
	float y = waveInfo.start_y;
	const char* c = squad.squad;
	for (int j = 0, indexInWave = 0; j < squad.numRows; ++j, y += waveInfo.dy) {
		float x = x0;
		for (int k = 0; k < squad.numCols; k++, x += waveInfo.dx, ++indexInWave)
		{
			if (*c != '0') 
			{
				const AlienPrefab& alienPrefab = GetAlienPrefab(*c - '0' - 1);
				Vector2D velocity = { waveInfo.initialSpeed * waveInfo.initialDirection, config.alienDownVelocity };
				Alien alien = NewAlien( { x, y }, velocity, alienPrefab, world.rndFloat01(world.rGen) );
				alien.waveIndex = waveIndex;
				alien.indexInWave = indexInWave;
				world.AddAlienShip(alien);
				++wave.numAliens;
				wave.mask[indexInWave] = 1;
			}
			else 
			{
				wave.mask[indexInWave] = 0;
			}
			++c;
		}
	}
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
		game.score[s] = 0;
	}
	CreatePlayers(game, game.world, mode);
	game.world.Restart();
	game.world.Update(0.f, game.scriptModule);
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

	if (mode == Game::Mode::p1 || mode == Game::Mode::p1cpu2 || mode == Game::Mode::p1p2)
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
	else if (mode == Game::Mode::p1cpu2 || mode == Game::Mode::cpu1cpu2)
	{
		input1 = std::make_unique<RndInput>(game.rGen);
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


}
