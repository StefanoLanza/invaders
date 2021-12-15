#pragma once

#include <cstddef>

enum class AlienScriptId
{
	alien = 0
};
constexpr size_t numAlienScripts = static_cast<size_t>(AlienScriptId::alien) + 1;


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
