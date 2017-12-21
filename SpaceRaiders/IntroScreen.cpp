#include "IntroScreen.h"
#include "GameStates.h"
#include "Input.h"
#include "Renderer.h"
#include "Images.h"
#include <cassert>


struct IntroScreenData
{
	float accumTime = 0.f;
};
IntroScreenData introScreenData;


int IntroScreen(Game& game, void* data, float dt)
{
	IntroScreenData& screenData = *(IntroScreenData*)data;
	GameStateId nextState = GameStateId::intro;
	screenData.accumTime += dt;
	if (AnyKeyJustPressed() || screenData.accumTime > 8.f)
	{
		nextState = GameStateId::running;
		screenData.accumTime = 0.f;
	}
	return (int)nextState;
}


void DisplayIntroScreen(Renderer& renderer, const void* data)
{
	renderer.DrawImage(GetImage(ImageId::planet), 0, 8, Color::white, Renderer::Alignment::centered, Renderer::Alignment::top);
	static const char* str[] =
	{
		"",
		"Planet IKA is under attack!",
		"Destroy the alien invaders before they reach the ground",
		"Save the planet and earn eternal glory",
		""
	};
	constexpr int numRows = _countof(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[2])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, r == 0 ? Color::greenIntense : Color::white);
	}
}
