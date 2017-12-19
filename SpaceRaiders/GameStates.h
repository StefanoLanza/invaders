#pragma once


enum class GameStateId
{
	start,
	intro,
	running,
	paused,
	over,
	victory,
	quit,
	count
};


class Game;
class Renderer;

GameStateId RunGameState(GameStateId id, Game& game, float dt);
void DrawGameState(GameStateId id, Renderer& renderer);
