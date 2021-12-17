#pragma once


struct Game;
class Console;

// FIXME Should be part of the engine
struct GameState
{
	// FIXME Enter and Exit function ? Start for example should destroy all objects
	using Enter = void (*)(void* data, Game& game, int currentState);
	using Run = int (*)(Game& game, void* data, float dt);
	using Draw = void (*)(Console& renderer, const void* data);

	void* data;
	Run   runFunc;
	Draw  drawFunc;
	Enter enter;
};


void RegisterGameState(Game& game, void* data, GameState::Run run, GameState::Draw draw, GameState::Enter enter);
void EnterGameState(Game& game, int newStateIndex);
void RunGameState(Game& game, float dt);
void DrawGameState(const Game& game, Console& renderer);
