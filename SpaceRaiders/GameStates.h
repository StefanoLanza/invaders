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


// Per game
class Game;
void RegisterGameStates(Game&);
