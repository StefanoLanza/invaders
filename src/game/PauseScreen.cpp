#include "PauseScreen.h"
#include "GameStates.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include "Images.h"
#include <cassert>
#include <cstring>


int PauseScreen(Game& game, void* data, float dt)
{
	GameStateId newState = GameStateId::paused;
	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::start;
	}
	else if (KeyJustPressed(KeyCode::enter))
	{
		newState = GameStateId::running;
	}
	return (int)newState;
}


void DisplayPauseScreen(Console& console, const void* data)
{
	static const char* str[] =
	{
		"Press ENTER to continue game",
		"Press ESC to go back to main menu",
	};
	constexpr int numRows = static_cast<int>(std::size(str));
	const IVector2D& bounds = console.GetBounds();
	const int width = 50;
	const int height = 10;
	const int top = (bounds.y - height) / 2; // centered
	const int left = (bounds.x - width) / 2;
	const int textCol = left + 4;
	const int textRow = top + 3;
	console.DrawBorder(left - 1, top - 1, width + 2, height + 2, Color::white);
	console.DrawRectangle(left, top, width, height, Color::black);

	for (int r = 0; r < numRows; ++r)
	{
		console.DisplayText(str[r], textCol, textRow + r * 2, Color::white);
	}
}

