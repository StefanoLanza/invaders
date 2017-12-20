#include "GameStateMgr.h"
#include "Game.h"
#include <cassert>
#include <functional>


void RegisterGameState(Game& game, void* data, GameState::Run run, GameState::Draw draw)
{
	game.states.push_back( { data, run, draw } );
}


int RunGameState(int id, Game& game, float dt)
{
	assert(id >= 0 && id < game.states.size());
	const GameState& state = game.states[id];
	return state.runFunc ? state.runFunc(game, state.data, dt) : id;
}


void DrawGameState(int id, const Game& game, Renderer& r)
{
	assert(id >= 0 && id < game.states.size());
	auto fnc = game.states[id].drawFunc;
	if (fnc)
	{
		fnc(r);
	}
}
