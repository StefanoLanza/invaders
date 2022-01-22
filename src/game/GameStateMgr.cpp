#include "GameStateMgr.h"
#include "Game.h"
#include <cassert>
#include <functional>


void RegisterGameState(Game& game, void* data, GameState::Run run, GameState::Draw draw, GameState::Enter enter, GameState::Exit exit)
{
	game.states.push_back( { data, run, draw, enter, exit } );
}


void RunGameState(Game& game, float dt)
{
	assert(game.stateId >= 0 && game.stateId < (int)game.states.size());
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


void DrawGameState(const Game& game, Console& console)
{
	const GameState& state = game.states[game.stateId];
	if (state.drawFunc)
	{
		state.drawFunc(console, state.data);
	}
}


void EnterGameState(Game& game, int newStateIndex)
{
	assert(newStateIndex >= 0 && newStateIndex < (int)game.states.size());
	if (game.stateId >= 0)
	{
		const GameState& currState = game.states[game.stateId];
		if (currState.exit)
		{
			currState.exit(currState.data);
		}
	}

	const GameState& newState = game.states[newStateIndex];
	if (newState.enter && newStateIndex != game.stateId)
	{
		newState.enter(newState.data, game, game.stateId);
	}
	game.stateId = newStateIndex;
}
