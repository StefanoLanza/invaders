#pragma once

#include <vector>


class Game;
class Renderer;


struct GameState
{
	// FIXME Enter and Exit function ? Start for example should destroy all objects
	using Run = int (*)(Game& game, void* data, float dt);
	using Draw = void (*)(Renderer& renderer); // FIXME data

	void* data;
	Run   runFunc;
	Draw  drawFunc;
};


void RegisterGameState(Game& game, void* data, GameState::Run run, GameState::Draw draw);
int RunGameState(int id, Game& game, float dt);
void DrawGameState(int id, const Game& game, Renderer& renderer);
