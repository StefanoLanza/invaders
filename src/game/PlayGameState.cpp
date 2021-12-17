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
#include "Prefabs.h"
#include <engine/Input.h>
#include <engine/MessageLog.h>
#include "GameEvents.h"
#include <engine/Console.h>
#include <engine/CollisionSpace.h>
#include <cassert>
#include <functional>


struct PlayGameStateData
{
	CollisionSpace collisionSpace;
	int            levelIndex = -1;
	float          levelTime;
	bool           showLevel;
	int            eventIndex;
	int            numHits;
};
PlayGameStateData playGameStateData;


namespace
{


struct CollisionContext
{
	PlayField*  world;
	const GameConfig* gameConfig;
	MessageLog* messageLog;
	PlayGameStateData* stateData;
};

// Collision matrix
void CheckCollisions(PlayField& world, CollisionSpace& collisionSpace, PlayGameStateData& stateData);
// Collision callbacks
void Collision_LaserVSLaser(const CollisionContext&, void* ud0, void* ud1);
void Collision_AlienVSLaser(const CollisionContext&, void* ud0, void* ud1);
void Collision_PlayerVSLaser(const CollisionContext&, void* ud0, void* ud1);
void Collision_PlayerVSPowerUp(const CollisionContext&, void* ud0, void* ud1);
void Collision_PlayerVSAlien(const CollisionContext&, void* ud0, void* ud1);
void Collision_LaserVSWall(const CollisionContext&, void* ud0, void* ud1);
void Collision_AlienVSWall(const CollisionContext&, void* ud0, void* ud1);
void Collision_AlienVSAlien(const CollisionContext&, void* ud0, void* ud1);
void ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig);
//void SpawnBoss(const BossInfo& boss);
void SetLevel(int levelIndex, PlayGameStateData& data);
void RestartGame(PlayGameStateData& stateData, Game& game);
void ProcessEvent(const Event& event, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig);
void SpawnAlienWave(const AlienWave& wave, PlayField& world, const GameConfig& config);
void SpawnBoss(const BossInfo& boss, PlayField& world, const GameConfig& config);
void SpawnWall(PlayField& world, const WallInfo& wall);
void Start(Game& game, const GameConfig& config, Game::Mode mode);
void CreatePlayers(Game& game, PlayField& world, Game::Mode mode);

}


void EnterPlayGame(void* data, Game& game, int currentState)
{
	PlayGameStateData& stateData = *(PlayGameStateData*)data;
	if (currentState != (int)GameStateId::paused)
	{
		RestartGame(stateData, game);
	}
}


int PlayGame(Game& game, void* data, float dt)
{
	GameStateId newState = GameStateId::running;
	PlayGameStateData& stateData = *(PlayGameStateData*)data;
	PlayField& world = game.world;
	if (world.GetPlayers().empty())
	{
		game.messageLog.Clear();
		newState = GameStateId::over;
		return (int)newState;
	}

	// Update score
	for (const auto& player : world.GetPlayers())
	{
		assert(player.id < game.maxPlayers);
		game.score[player.id] = player.score;
	}

	const Level& level = GetLevel(stateData.levelIndex);
	if (stateData.eventIndex >= level.numEvents && world.NoAliens())
	{
		if (stateData.levelIndex < GetNumLevels() - 1)
		{
			// Next level
			SetLevel(stateData.levelIndex + 1, stateData);
		}
		else
		{
			newState = GameStateId::victory;
		}
		return (int)newState;
	}

	stateData.levelTime += dt;
	if (stateData.levelTime > 2.f)
	{
		stateData.showLevel = false;
	}
	if (stateData.eventIndex < level.numEvents && stateData.levelTime > level.events[stateData.eventIndex].time)
	{
		ProcessEvent(level.events[stateData.eventIndex], game.messageLog, game.world, game.config);
		++stateData.eventIndex;
	}

	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::paused;
	}
	if (! stateData.showLevel)
	{
		world.Update(dt, game.scriptModule);
		CheckCollisions(world, stateData.collisionSpace, stateData);
	}

	return (int)newState;
}


void DisplayPlayGame(Console& renderer, const void* data)
{
	const PlayGameStateData& stateData = *(const PlayGameStateData*)data;
	if (stateData.showLevel)
	{
		ImageId imageId = (ImageId)(stateData.levelIndex + (int)GameImageId::_1);
		renderer.DrawImage(GetImage(imageId), 0, 2, Color::yellowIntense, ImageAlignment::centered, ImageAlignment::centered);
		renderer.DrawImage(GetImage(GetImageId(GameImageId::level)), 0, -4, Color::yellowIntense, ImageAlignment::centered, ImageAlignment::centered);
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
		collisionSpace.Add(GetCollider(alien));
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

	CollisionInfo collisions[64];
	const int nc = collisionSpace.Execute(collisions, (int)std::size(collisions));
	CollisionInfo* c = collisions;

	using Callback = void (*) (const CollisionContext&, void*, void*);
	struct CallbackInfo
	{
		ColliderId id0;
		ColliderId id1;
		Callback   fnc;
	};
	static const CallbackInfo callbacks[] = 
	{
		{ ColliderId::player, ColliderId::alienLaser, Collision_PlayerVSLaser },
		{ ColliderId::player, ColliderId::powerUp, Collision_PlayerVSPowerUp },
		{ ColliderId::alien, ColliderId::alien, Collision_AlienVSAlien },
		{ ColliderId::alien, ColliderId::playerLaser, Collision_AlienVSLaser },
		{ ColliderId::alien, ColliderId::wall, Collision_AlienVSWall },
		{ ColliderId::playerLaser, ColliderId::alienLaser, Collision_LaserVSLaser },
		{ ColliderId::playerLaser, ColliderId::wall, Collision_LaserVSWall },
		{ ColliderId::player, ColliderId::alien, Collision_PlayerVSAlien },
	};

	const CollisionContext context =
	{
		&world,
		&world.config,
		&world.messageLog,
		&stateData
	};

	for (int i = 0; i < nc; ++i, ++c)
	{
		for (const auto& cbk : callbacks)
		{
			// Collision matrix
			if (c->id0 == cbk.id0 && c->id1 == cbk.id1)
			{
				cbk.fnc(context, c->ud0, c->ud1);
				break;
			}
			if (c->id0 == cbk.id1 && c->id1 == cbk.id0)
			{
				cbk.fnc(context, c->ud1, c->ud0);
				break;
			}
		}
	}
}


void Collision_LaserVSLaser(const CollisionContext& context, void* ud0, void* ud1)
{
	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Laser& alienLaser = *static_cast<Laser*>(ud1);
	// Spawn explosion, kill this and the alien laser
	Vector2D pos = Lerp(playerLaser.body.pos, alienLaser.body.pos, 0.5f);
	context.world->AddExplosion(pos, context.gameConfig->explosionTimer);
	DestroyLaser(alienLaser);
	DestroyLaser(playerLaser);
}


void Collision_AlienVSLaser(const CollisionContext& context, void* ud0, void* ud1)
{
	Alien& alien = *(Alien*)ud0;
	Laser& playerLaser = *(Laser*)ud1;
	DestroyLaser(playerLaser);
	HitAlien(alien);
	//Spawn explosion 
	context.world->AddScore( alien.state == Alien::State::normal ? 10 : 20, playerLaser.ownerId);
	context.world->AddExplosion(alien.body.pos, context.gameConfig->explosionTimer);
	if (alien.state == Alien::State::dead)  // FIXME hits or destroyed aliens?
	{
		++context.stateData->numHits;
		if (context.stateData->numHits >= context.gameConfig->powerUpHits)
		{
			context.world->SpawnRandomPowerUp(alien.body.pos);
			context.stateData->numHits =  0;
		}
	}

	// FIXME
	//static int b = 0; if (!b++) {
	//context.world->SpawnPowerUp( { context.world->bounds.x/2, 10.f }, PowerUp::Type::invulnerability);
	//context.world->SpawnPowerUp( { context.world->bounds.x/2 - 10, 10.f }, PowerUp::Type::invulnerability);
//}
}


// FIXME Collision with shield actually
void Collision_PlayerVSLaser(const CollisionContext& context, void* ud0, void* ud1)
{
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


void Collision_PlayerVSAlien(const CollisionContext& context, void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	Alien& alien = *(Alien*)ud1;
	//Spawn explosion, destroy player and alien
	DestroyAlien(alien);
	if (! context.gameConfig->godMode)
	{
		player.Destroy();
	}
	context.world->AddExplosion(alien.body.pos, context.gameConfig->explosionTimer);
}


void Collision_PlayerVSPowerUp(const CollisionContext& context, void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	PowerUp& powerUp = *(PowerUp*)ud1;
	ActivatePowerUp(player, powerUp, *context.messageLog, *context.world, *context.gameConfig);
	Destroy(powerUp);
}


void Collision_LaserVSWall(const CollisionContext& context, void* ud0, void* ud1)
{
	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	DestroyLaser(playerLaser);
	HitWall(wall);
	const Vector2D pos = { wall.pos.x,  wall.pos.y + 1.f };
	context.world->AddExplosion(pos, context.gameConfig->explosionTimer);
}


void Collision_AlienVSWall(const CollisionContext& /*context*/, void* ud0, void* ud1)
{
	Alien& alien = *static_cast<Alien*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	Alien_AvoidWall(alien, wall.pos);
}


void Collision_AlienVSAlien(const CollisionContext& /*context*/, void* /*ud0*/, void* /*ud1*/)
{
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


void SetLevel(int levelIndex, PlayGameStateData& data)
{
	//FIXME game.world.DestroyWalls();
	data.levelIndex = levelIndex;
	data.levelTime = 0.f;
	data.eventIndex = 0;
	data.showLevel = true;
	data.numHits = 0;
}


void RestartGame(PlayGameStateData& stateData, Game& game)
{
	Start(game, game.config, game.mode);
	SetLevel(0, stateData);
}


void ProcessEvent(const Event& event, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig)
{
	switch (event.type)
	{
		case EventType::message:
			messageLog.AddMessage((const char*)event.data, Color::yellowIntense);
			break;
		case EventType::spawnWave:
			SpawnAlienWave(*(const AlienWave*)event.data, world, gameConfig);
			break;
		case EventType::spawnWalls:
			SpawnWall(world, *(const WallInfo*)event.data);
			break;
		case EventType::boss:
			SpawnBoss(*(const BossInfo*)event.data, world, gameConfig);
			break;
		case EventType::wait:
			break;
		default:
			break;
	};
}


void SpawnWall(PlayField& world, const WallInfo& wall)
{
	float x = wall.x;
	const float y = wall.y;
	for (int k = 0; k < wall.n; k++)
	{
		world.AddWall( { x, y } );
		x += 4.f;
	}
}


void SpawnBoss(const BossInfo& boss, PlayField& world, const GameConfig& config)
{
	const AlienPrefab& prefab = GetBossPrefab(boss.alienType);
	world.AddAlienShip( NewAlien( { boss.x, boss.y }, { prefab.speed, config.alienDownVelocity }, prefab, prefab ) );
}


void SpawnAlienWave(const AlienWave& wave, PlayField& world, const GameConfig& config)
{
	float x = (world.GetBounds().x - (wave.n * wave.dx)) / 2.f;
	float y = wave.y;
	for (int k = 0; k < wave.n; k++, x += wave.dx)
	{
		float direction = wave.direction;
		const AlienPrefab& alienPrefab = GetAlienPrefab(wave.alienType);
		const AlienPrefab& betterAlienPrefab = GetAlienPrefab(wave.betterAlienType);
		Vector2D velocity = { alienPrefab.speed * direction * config.alienSpeedMul, config.alienDownVelocity };
		world.AddAlienShip( NewAlien( { x, y }, velocity, alienPrefab, betterAlienPrefab ) );
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
		input0 = std::make_unique<KeyboardInput>(KeyCode::left, KeyCode::right);
	}
	else
	{
		input0 = std::make_unique<RndInput>(game.rGen);
	}
	if (mode == Game::Mode::p1p2)
	{
		input1 = std::make_unique<KeyboardInput>(KeyCode::A, KeyCode::D);
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
