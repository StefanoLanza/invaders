#pragma once

#include <cstddef>

enum class AlienScriptId
{
	alien = 0,
	sentinel = 1,
	boss = 2
};
constexpr size_t numAlienScripts = 3;


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
