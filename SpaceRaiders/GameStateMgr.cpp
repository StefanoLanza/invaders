#include "GameStateMgr.h"
#include "Game.h"
#include <cassert>
#include <functional>


void RegisterGameState(Game& game, void* data, GameState::Run run, GameState::Draw draw, GameState::Enter enter)
{
	game.states.push_back( { data, run, draw, enter } );
}


void RunGameState(Game& game, float dt)
{
	assert(game.stateId >= 0 && game.stateId < game.states.size());
	const GameState& state = game.states[game.stateId];
	if (state.runFunc)
	{
		const int currentState = game.stateId;
		const int newStateId = state.runFunc(game, state.data, dt);
		if (newStateId != currentState)
		{
			EnterGameState(game, newStateId);
		}
	}
}


void DrawGameState(int id, const Game& game, Renderer& r)
{
	assert(id >= 0 && id < game.states.size());
	const GameState& state = game.states[game.stateId];
	if (state.drawFunc)
	{
		state.drawFunc(r, state.data);
	}
}


void EnterGameState(Game& game, int newStateIndex)
{
	assert(newStateIndex >= 0 && newStateIndex < game.states.size());
	const GameState& state = game.states[newStateIndex];
	if (state.enter)
	{
		state.enter(state.data, game.stateId);
	}
	game.stateId = newStateIndex;
}
