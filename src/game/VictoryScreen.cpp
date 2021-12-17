#include "VictoryScreen.h"
#include "GameStates.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include <cassert>
#include <cstring>

int VictoryScreen(Game& game, void* data, float dt)
{
	GameStateId newState = GameStateId::victory;
	if (AnyKeyJustPressed())
	{
		newState = GameStateId::start;
	}
	return (int)newState;
}


void DisplayVictoryScreen(Console& renderer, const void* data)
{
	static const char* str[] =
	{
		"",
		"You saved the planet!",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = std::size(str);
	const int row = (renderer.bounds.y - numRows) / 2; // centered
	const int col = (renderer.bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}

