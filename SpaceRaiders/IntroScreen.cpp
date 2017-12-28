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


void EnterIntroScreen(void* data, Game& game, int currentState)
{
	IntroScreenData& screenData = *(IntroScreenData*)data;
}


int IntroScreen(Game& game, void* data, float dt)
{
	IntroScreenData& screenData = *(IntroScreenData*)data;
	GameStateId nextState = GameStateId::intro;
	screenData.accumTime += dt;
	if (AnyKeyJustPressed() || screenData.accumTime > 4.f)
	{
		nextState = GameStateId::running;
		screenData.accumTime = 0.f;
	}
	return (int)nextState;
}


void DisplayIntroScreen(Renderer& renderer, const void* data)
{
	const IntroScreenData& screenData = *(IntroScreenData*)data;

	renderer.DrawImage(GetImage(ImageId::planet), 0, 8, Color::greenIntense, ImageAlignment::centered, ImageAlignment::top);
#if 0
	static const char* str[] =
	{
		"",
		"The alien invaders are attacking!",
		"They want to kidnap princess Irene",
		"who holds the secret of Christmas in her heart.",
		"Destroy them before they land",
		"Save Irene, save Christmas and earn eternal glory !",
		""
	};

#else
	static const char* str[] =
	{
		"The invaders from planet IKA are attacking!",
		"Avoid their lethal lasers",
		"and destroy them before they reach the ground",
		"Save us all and earn eternal glory !"
	};
#endif
	constexpr int numRows = _countof(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[0])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}
