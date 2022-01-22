#include "PauseScreen.h"
#include "GameStates.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include "GameStateMgr.h"
#include "Images.h"
#include <cassert>
#include <cstring>


namespace
{

struct PauseScreenData
{
	int selection = 0;
};
PauseScreenData screenData;


void EnterPauseScreen(void* data, Game& game, int currentState) 
{
	screenData.selection = 0;
}

int PauseScreen(Game& game, void* data_, float dt)
{
	GameStateId newState = GameStateId::paused;
	PauseScreenData& data = *(PauseScreenData*)data_;
	if (KeyJustPressed(KeyCode::up))
	{
		data.selection = (data.selection - 1 + 2) % 2;
	}
	else if (KeyJustPressed(KeyCode::down))
	{
		data.selection = (data.selection + 1) % 2;
	}
	if (KeyJustPressed(KeyCode::enter))
	{
		if (data.selection == 0)
		{
			newState = GameStateId::play;
		}
		else if (data.selection == 1)
		{
			newState = GameStateId::start;
		}
	}
	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::play;
	}
	return (int)newState;
}


void DisplayPauseScreen(Console& console, const void* data)
{
	static const char* str[] =
	{
		"CONTINUE GAME",
		"EXIT GAME",
	};
	constexpr int numRows = static_cast<int>(std::size(str));
	const IVector2D& bounds = console.GetBounds();
	const int boxWidth = 50;
	const int boxHeight = 10;
	const int top = (bounds.y - boxHeight) / 2; // centered
	const int left = (bounds.x - boxWidth) / 2;
	const int textCol = left + 16;
	const int textRow = top + 3;
	console.DrawBorder(left - 1, top - 1, boxWidth + 2, boxHeight + 2, Color::white);
	console.DrawRectangle(left, top, boxWidth, boxHeight, Color::black);

	for (int r = 0; r < numRows; ++r)
	{
		console.DisplayText(str[r], textCol, textRow + r * 2, Color::white);
	}
	console.DisplayText(">", textCol - 3, textRow + screenData.selection * 2, Color::white);
}

}

void RegisterPauseScreen(Game& game)
{
	RegisterGameState(game, &screenData, PauseScreen, DisplayPauseScreen, EnterPauseScreen, nullptr);
}
