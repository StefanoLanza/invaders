#include "GameOverState.h"
#include "GameStates.h"
#include "Images.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include <cassert>


int GameOverMenu([[maybe_unused]] Game& game, [[maybe_unused]] void* data, [[maybe_unused]] float dt)
{
	GameStateId newState = GameStateId::over;
	if (AnyKeyJustPressed())
	{
		newState = GameStateId::start;
	}
	return (int)newState;
}


void DisplayGameOver(Console& console, [[maybe_unused]] const void* gameState)
{
	const int width = 50;
	const int height = 10;
	const IVector2D& bounds = console.GetBounds();
	const int top = (bounds.y - height) / 2; // centered
	const int left = (bounds.x - width) / 2;
	const int textCol = left + 5;
	const int textRow = top + 3;
	console.DrawBorder(left - 1, top - 1, width + 2, height + 2, Color::white);
	console.DrawRectangle(left, top, width, height, Color::black);
	console.DrawImage(GetImage(GameImageId::gameOver), textCol, textRow, Color::white, ImageAlignment::left, ImageAlignment::top);
}
