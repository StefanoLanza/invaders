#include "GameStates.h"
#include "Game.h"
#include "GameConfig.h"
#include "PlayField.h"
#include "Player.h"
#include "Alien.h"
#include "Laser.h"
#include "PowerUp.h"
#include "Wall.h"
#include "Input.h"
#include "MessageLog.h"
#include "GameEvents.h"
#include "Renderer.h"
#include "CollisionSpace.h"
#include <cassert>
#include <functional>


namespace
{
// Images
const wchar_t ikaLogo[] =
{
	#include "IKA.txt"
};
const wchar_t raidersTxt[] =
{
	#include "raiders.txt"
};
const Image ikaImg = { ikaLogo, 25, 6 };
const Image raidersImg = { raidersTxt, 46, 6 };

GameStateId StartMenu(Game& game, void* gameState, float dt);
GameStateId Intro(Game& game, void* gameState, float dt);
GameStateId VictoryMenu(Game& game, void* gameState, float dt);
GameStateId PauseMenu(Game& game, void* gameState, float dt);
GameStateId GameOverMenu(Game& game, void* gameState, float dt);
GameStateId RunGame(Game& game, void* gameState, float dt);
void DrawStartMenu(Renderer& renderer);
void DisplayIntro(Renderer& renderer);
void DisplayPauseMenu(Renderer& renderer);
void DisplayVictory(Renderer& renderer);
void DisplayGameOver(Renderer& renderer);


struct RunGameState
{
	CollisionSpace collisionSpace; // { std::make_unique<CollisionSpace>() }
};
RunGameState runGameStateData;

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


struct GameState
{
	// FIXME Enter and Exit function ? Start for example should destroy all objects
	using RunFunc = GameStateId (*)(Game& game, void* data, float dt);
	using DrawFunc = void (*)(Renderer& renderer);

	void*    data;
	RunFunc  runFunc;
	DrawFunc drawFunc;
};

const GameState gameStates[(int)GameStateId::count] =
{
	{ nullptr, StartMenu, DrawStartMenu },
	{ nullptr, Intro, DisplayIntro },
	{ &runGameStateData, RunGame, nullptr },
	{ nullptr, PauseMenu, DisplayPauseMenu },
	{ nullptr, GameOverMenu, DisplayGameOver },
	{ nullptr, VictoryMenu, DisplayVictory },
	{ nullptr, nullptr }
};

}


GameStateId RunGameState(GameStateId id, Game& game, float dt)
{
	assert((int)id < _countof(gameStates));
	const GameState& state = gameStates[(int)id];
	return state.runFunc ? state.runFunc(game, state.data, dt) : id;
}

void DrawGameState(GameStateId id, Renderer& r)
{
	assert((int)id < _countof(gameStates));
	auto fnc = gameStates[(int)id].drawFunc;
	if (fnc)
	{
		fnc(r);
	}
}


namespace
{

GameStateId StartMenu(Game& game, void* data, float /*dt*/)
{
	GameStateId newState;
	game.numPlayers = 0;
	game.messageLog.Clear();
	game.world.DestroyAll(); // FIXME OnEnter

	if (KeyJustPressed(KeyCode::_1))
	{
		game.Start(Game::Mode::p1);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_2))
	{
		game.Start(Game::Mode::cpu1);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_3))
	{
		game.Start(Game::Mode::p1p2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_4))
	{
		game.Start(Game::Mode::p1cpu2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_5))
	{
		game.Start(Game::Mode::cpu1cpu2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::quit;
	}
	else
	{
		newState = GameStateId::start;
	}
	return newState;
}


GameStateId Intro(Game& game, void* data, float dt)
{
	GameStateId nextState = GameStateId::intro;
	game.accumTime += dt;
	if (AnyKeyJustPressed() || game.accumTime > 8.f)
	{
		game.SetLevel(0);
		nextState = GameStateId::running;
	}
	return nextState;
}


GameStateId VictoryMenu(Game& game, void* data, float dt)
{
	GameStateId newState = GameStateId::victory;
	if (AnyKeyJustPressed())
	{
		newState = GameStateId::start;
	}
	return newState;
}


GameStateId PauseMenu(Game& game, void* data, float dt)
{
	GameStateId newState = GameStateId::paused;
	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::start;
	}
	else if (KeyJustPressed(KeyCode::enter))
	{
		newState = GameStateId::running;
	}
	return newState;
}


GameStateId GameOverMenu(Game& game, void* data,  float dt)
{
	GameStateId newState = GameStateId::over;
	if (AnyKeyJustPressed())
	{
		newState = GameStateId::start;
	}
	return newState;
}


GameStateId RunGame(Game& game, void* data, float dt)
{
	GameStateId newState = GameStateId::running;
	RunGameState* stateData = (RunGameState*)data;
	PlayField& world = game.world;
	if (world.GetPlayers().empty())
	{
		game.messageLog.Clear();
		newState = GameStateId::over;
		return newState;
	}

	// Update score
	for (const auto& player : world.GetPlayers())
	{
		assert(player.GetId() < game.maxPlayers);
		game.score[player.GetId()] = player.GetScore();
	}

	const Level& level = GetLevel(game.levelIndex);
	if (game.eventIndex >= level.numEvents && world.NoAliens())
	{
		if (game.levelIndex < GetNumLevels() - 1)
		{
			game.NextLevel();
		}
		else
		{
			newState = GameStateId::victory;
		}
		return newState;
	}

	game.accumTime += dt;
	if (game.eventIndex < level.numEvents && game.accumTime > level.events[game.eventIndex].time)
	{
		game.ProcessEvent(level.events[game.eventIndex]);
		++game.eventIndex;
	}

	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::paused;
	}
	world.Update(dt, game.scriptModule);
	CheckCollisions(world, stateData->collisionSpace);

	return newState;
}


void DrawStartMenu(Renderer& renderer)
{
	renderer.DrawImage(ikaImg, 0, 0, Color::greenIntense, Renderer::Alignment::centered, Renderer::Alignment::top);
	renderer.DrawImage(raidersImg, 0, 7, Color::greenIntense, Renderer::Alignment::centered,  Renderer::Alignment::top);

	static const char* str[] =
	{
		"",
		"Single Player",
		"  Press 1 to start one player game",
		"  Press 2 to start one CPU game",
		"",
		"Multi Player",
		"  Press 3 to start two players game",
		"  Press 4 to start player and CPU game",
		"  Press 5 to start two CPUs game",
		"",
		"Press ESC to quit"
	};
	constexpr int numRows = _countof(str);
	const int row = 16;
	const int col = (renderer.GetBounds().x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


void DisplayIntro(Renderer& renderer)
{
	renderer.DrawImage(GetImage(ImageId::planet), 0, 8, Color::white, Renderer::Alignment::centered, Renderer::Alignment::top);
	static const char* str[] =
	{
		"",
		"Planet IKA is under attack!",
		"Destroy the alien invaders before they reach the ground",
		"Save the planet and earn eternal glory",
		""
	};
	constexpr int numRows = _countof(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[2])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, r == 0 ? Color::greenIntense : Color::white);
	}
}


void DisplayPauseMenu(Renderer& renderer)
{
	static const char* str[] =
	{
		"",
		"Press ENTER to continue game",
		"Press ESC to go back to main menu",
		""
	};
	constexpr int numRows = _countof(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[1])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


void DisplayVictory(Renderer& renderer)
{
	static const char* str[] =
	{
		"",
		"You saved the planet!",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderer.bounds.y - numRows) / 2; // centered
	const int col = (renderer.bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


void DisplayGameOver(Renderer& renderer)
{
	static const char* str[] =
	{
		"",
		"Game Over",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderer.bounds.y - numRows) / 2; // centered
	const int col = (renderer.bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


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
		collisionSpace.Add(Alien_GetCollider(alien));
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
	Alien_DecreaseHealth(alien); // kill the alien that we hit
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


void Collision_PlayerVSLaser(const CollisionContext& context, void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	Laser& alienLaser = *(Laser*)ud1;
	//Spawn explosion, destroy player and laser
	DestroyLaser(alienLaser);
	if (! context.gameConfig->godMode)
	{
		player.Destroy();
	}
	context.world->AddExplosion(player.GetPosition(), context.gameConfig->explosionTimer);
}


void Collision_PlayerVSAlien(const CollisionContext& context, void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	Alien& alien = *(Alien*)ud1;
	//Spawn explosion, destroy player and alien
	Alien_Destroy(alien);
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


void Collision_AlienVSWall(const CollisionContext& context, void* ud0, void* ud1)
{
	Alien& alien = *static_cast<Alien*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	Alien_AvoidWall(alien, wall.GetPosition());
}


void Collision_AlienVSAlien(const CollisionContext& context, void* ud0, void* ud1)
{
	Alien& alien0 = *static_cast<Alien*>(ud0);
	Alien& alien1 = *static_cast<Alien*>(ud1);
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


}
