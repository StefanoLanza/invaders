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
#include "Input.h"
#include "MessageLog.h"
#include "GameEvents.h"
#include "Renderer.h"
#include "CollisionSpace.h"
#include <cassert>
#include <functional>


struct PlayGameStateData
{
	CollisionSpace collisionSpace;
	int            levelIndex = -1;
	float          levelTime;
	bool           showLevel;
	int            eventIndex;
};
PlayGameStateData playGameStateData;


namespace
{


struct CollisionContext
{
	PlayField*  world;
	const GameConfig* gameConfig;
	MessageLog* messageLog;
};

// Collision matrix
void CheckCollisions(PlayField& world, CollisionSpace& collisionSpace);
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
void RestartGame(PlayGameStateData& stateData);

}


void EnterPlayGame(void* data, int currentState)
{
	PlayGameStateData& stateData = *(PlayGameStateData*)data;
	if (currentState != (int)GameStateId::paused)
	{
		RestartGame(stateData);
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
		assert(player.GetId() < game.maxPlayers);
		game.score[player.GetId()] = player.GetScore();
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
		game.ProcessEvent(level.events[stateData.eventIndex]);
		++stateData.eventIndex;
	}

	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::paused;
	}
	world.Update(dt, game.scriptModule);
	CheckCollisions(world, stateData.collisionSpace);

	return (int)newState;
}


void DisplayPlayGame(Renderer& renderer, const void* data)
{
	const PlayGameStateData& stateData = *(const PlayGameStateData*)data;
	if (stateData.showLevel)
	{
		ImageId imageId = (ImageId)(stateData.levelIndex + (int)ImageId::_1);
		renderer.DrawImage(GetImage(imageId), 0, 0, Color::yellowIntense, Renderer::Alignment::centered, Renderer::Alignment::centered);
		renderer.DrawImage(GetImage(ImageId::level), 0, -3, Color::yellowIntense, Renderer::Alignment::centered, Renderer::Alignment::centered);
	}
}


namespace
{

void CheckCollisions(PlayField& world, CollisionSpace& collisionSpace)
{
	// Collision detection
	collisionSpace.Clear();
	for (auto& player : world.players)
	{
		collisionSpace.Add(player.GetCollisionArea());
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
		collisionSpace.Add(powerUp.GetCollisionArea());
	}
	for (auto& wall : world.walls)
	{
		collisionSpace.Add(wall.GetCollisionArea());
	}

	CollisionInfo collisions[64];
	const int nc = collisionSpace.Execute(collisions, _countof(collisions));
	CollisionInfo* c = collisions;

	using Callback = std::function<void(const CollisionContext&, void*, void*)>;
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
		{ ColliderId::playerLaser, ColliderId::wall, Collision_LaserVSWall }
	};

	const CollisionContext context =
	{
		&world,
		&world.config,
		&world.messageLog
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
	if (alien.state == Alien::State::dead)
	{
		if (context.world->rndFloat01(context.world->rGen) < context.gameConfig->powerUpRate)
		{
			context.world->SpawnRandomPowerUp(alien.body.pos);
		}
	}

	// FIXME
	//static int b = 0; if (!b++) {
	//SpawnPowerUp( { bounds.x/2, 10.f }, PowerUp::Type::bomb);
	//SpawnPowerUp( { bounds.x/2 - 10, 10.f }, PowerUp::Type::doubleFire);
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
	powerUp.Destroy();
}


void Collision_LaserVSWall(const CollisionContext& context, void* ud0, void* ud1)
{
	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	DestroyLaser(playerLaser);
	wall.Hit();
	const Vector2D pos = { wall.GetPosition().x,  wall.GetPosition().y + 1.f };
	context.world->AddExplosion(pos, context.gameConfig->explosionTimer);
}


void Collision_AlienVSWall(const CollisionContext& /*context*/, void* ud0, void* ud1)
{
	Alien& alien = *static_cast<Alien*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	Alien_AvoidWall(alien, wall.GetPosition());
}


void Collision_AlienVSAlien(const CollisionContext& /*context*/, void* ud0, void* ud1)
{
//	Alien& alien0 = *static_cast<Alien*>(ud0);
//	Alien& alien1 = *static_cast<Alien*>(ud1);
//	alien0.body.pos = alien0.body.prevPos;
}


void ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog, PlayField& world, const GameConfig& gameConfig)
{
	switch (powerUp.GetType())
	{
	case PowerUp::speedBoost:
		messageLog.AddMessage("Speed Boost!");
		player.SetSpeedBoost(2);
		break;
	case PowerUp::fireBoost:
		messageLog.AddMessage("Fire Boost!");
		player.SetFireBoost(2);
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
	}
}


void SetLevel(int levelIndex, PlayGameStateData& data)
{
	//FIXME game.world.DestroyWalls();
	data.levelIndex = levelIndex;
	data.levelTime = 0.f;
	data.eventIndex = 0;
	data.showLevel = true;
}


void RestartGame(PlayGameStateData& stateData)
{
	SetLevel(0, stateData);
}


}
