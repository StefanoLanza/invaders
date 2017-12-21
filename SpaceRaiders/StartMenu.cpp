#include "StartMenu.h"
#include "GameStates.h"
#include "Game.h"
#include "Input.h"
#include "Renderer.h"
#include "Images.h"
#include "MessageLog.h"
#include "PlayField.h"
#include <cassert>


namespace
{
// Images
const wchar_t ikaLogo[] =
{
	#include "IKA.txt"
};
const wchar_t raidersTxt[] =
{
	#include "raiders.txt"
};
const Image ikaImg = { ikaLogo, 25, 6 };
const Image raidersImg = { raidersTxt, 46, 6 };
}

int StartMenu(Game& game, void* data, float /*dt*/)
{
	GameStateId newState;
	game.numPlayers = 0;
	game.messageLog.Clear();
	game.world.DestroyAll(); // FIXME OnEnter

	if (KeyJustPressed(KeyCode::_1))
	{
		game.Start(Game::Mode::p1);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_2))
	{
		game.Start(Game::Mode::cpu1);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_3))
	{
		game.Start(Game::Mode::p1p2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_4))
	{
		game.Start(Game::Mode::p1cpu2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_5))
	{
		game.Start(Game::Mode::cpu1cpu2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::quit;
	}
	else
	{
		newState = GameStateId::start;
	}
	return (int)newState;
}


void DisplayStartMenu(Renderer& renderer, const void* data)
{
	renderer.DrawImage(ikaImg, 0, 0, Color::greenIntense, Renderer::Alignment::centered, Renderer::Alignment::top);
	renderer.DrawImage(raidersImg, 0, 7, Color::greenIntense, Renderer::Alignment::centered,  Renderer::Alignment::top);

	static const char* str[] =
	{
		"",
		"Single Player",
		"  Press 1 to start one player game",
		"  Press 2 to start one CPU game",
		"",
		"Multi Player",
		"  Press 3 to start two players game",
		"  Press 4 to start player and CPU game",
		"  Press 5 to start two CPUs game",
		"",
		"Press ESC to quit"
	};
	constexpr int numRows = _countof(str);
	const int row = 16;
	const int col = (renderer.GetBounds().x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}

