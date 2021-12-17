#include "Game.h"
#include "GameStateMgr.h"


Game NewGame(PlayField& world, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog_, const ScriptModule& scriptModule_)
{
	Game game { config, rGen, messageLog_, scriptModule_, world, {}, Game::Mode::p1, -1, {}, {} };
	game.stateId = -1;
	game.numPlayers = 0;
	return game;
}
