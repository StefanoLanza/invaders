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


void DisplayPauseScreen(Renderer& renderer, const void* data)
{
	static const char* str[] =
	{
		"",
		"Press ENTER to continue game",
		"Press ESC to go back to main menu",
		""
	};
	constexpr int numRows = std::size(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[1])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}

