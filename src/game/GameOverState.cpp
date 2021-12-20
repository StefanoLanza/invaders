#include "GameOverState.h"
#include "GameStates.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include <cassert>
#include <cstring>


int GameOverMenu([[maybe_unused]] Game& game, [[maybe_unused]] void* data, [[maybe_unused]] float dt)
{
	GameStateId newState = GameStateId::over;
	if (AnyKeyJustPressed())
	{
		newState = GameStateId::start;
	}
	return (int)newState;
}


void DisplayGameOver(Console& renderer, [[maybe_unused]] const void* gameState)
{
	static const char* str[] =
	{
		"",
		"Game Over",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = (int)std::size(str);
	const int row = (renderer.bounds.y - numRows) / 2; // centered
	const int col = (renderer.bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}
