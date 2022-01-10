#pragma once

struct Game;
class PlayField;
struct GameConfig;
class CollisionSpace;

struct ScriptArgs
{
	float      dt;
	Game*      game;
	PlayField* world;
	const GameConfig* gameConfig;
	const CollisionSpace* collisionSpace;
};
