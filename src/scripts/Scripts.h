#pragma once

struct Game;
class PlayField;
struct GameConfig;

struct ScriptArgs
{
	float      dt;
	Game*      game;
	PlayField* world;
	const GameConfig* gameConfig;
};
