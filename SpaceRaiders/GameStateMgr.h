#pragma once


class Game;
class Renderer;


struct GameState
{
	// FIXME Enter and Exit function ? Start for example should destroy all objects
	using Enter = void (*)(void* data, int currentState);
	using Run = int (*)(Game& game, void* data, float dt);
	using Draw = void (*)(Renderer& renderer, const void* data);

	void* data;
	Run   runFunc;
	Draw  drawFunc;
	Enter enter;
};


void RegisterGameState(Game& game, void* data, GameState::Run run, GameState::Draw draw, GameState::Enter enter);
void EnterGameState(Game& game, int newStateIndex);
void RunGameState(Game& game, float dt);
void DrawGameState(int id, const Game& game, Renderer& renderer);
