#include "GameStates.h"
#include "GameStateMgr.h"
// States
#include "StartMenu.h"
#include "PauseScreen.h"
#include "PlayGameState.h"
#include "GameOverState.h"
#include "IntroScreen.h"
#include "VictoryScreen.h"



// FIXME In main
void RegisterGameStates(Game& game)
{
	RegisterGameState(game, nullptr, StartMenu, DisplayStartMenu, nullptr);
	RegisterGameState(game, &introScreenData, IntroScreen, DisplayIntroScreen, nullptr);
	RegisterGameState(game, &playGameStateData, PlayGame, nullptr, EnterPlayGame);
	RegisterGameState(game, nullptr, PauseScreen, DisplayPauseScreen, nullptr);
	RegisterGameState(game, nullptr, GameOverMenu, DisplayGameOver, nullptr);
	RegisterGameState(game, nullptr, VictoryScreen, DisplayVictoryScreen, nullptr);
	RegisterGameState(game, nullptr, nullptr, nullptr, nullptr);
}
