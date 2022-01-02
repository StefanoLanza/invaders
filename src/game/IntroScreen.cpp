#include "IntroScreen.h"
#include "GameStates.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include "Images.h"
#include <cassert>
#include <array>
#include <cstring>

struct IntroScreenData
{
	float accumTime = 0.f;
};
IntroScreenData introScreenData;


void EnterIntroScreen(void* data, Game& game, int currentState)
{
	IntroScreenData& screenData = *(IntroScreenData*)data;
	screenData.accumTime = 0.f;
}


int IntroScreen(Game& game, void* data, float dt)
{
	IntroScreenData& screenData = *(IntroScreenData*)data;
	GameStateId nextState = GameStateId::intro;
	screenData.accumTime += dt;
	constexpr float timeOut = 4.f;
	if (AnyKeyJustPressed() || screenData.accumTime > timeOut)
	{
		nextState = GameStateId::running;
		screenData.accumTime = 0.f;
	}
	return (int)nextState;
}


void DisplayIntroScreen(Console& renderer, const void* data)
{
	//const IntroScreenData& screenData = *(IntroScreenData*)data;

	renderer.DrawImage(GetImage(GameImageId::planet), 0, 8, Color::greenIntense, ImageAlignment::centered, ImageAlignment::top);
	static const char* str[] =
	{
		"The invaders from planet IKA are attacking!",
		"Avoid their lethal lasers",
		"and destroy them before they reach the ground",
		"Save us all and earn eternal glory !"
	};
	constexpr int numRows = static_cast<int>(std::size(str));
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[0])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}
