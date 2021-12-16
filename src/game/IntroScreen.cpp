#include "IntroScreen.h"
#include "GameStates.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include "Images.h"
#include <cassert>
#include <array>
#include <cstring>
#if SP_VERSION
#include "texts/Parrots.txt"
#endif

#if SP_EDITION
const wchar_t parrotsTxt[] =
{
	#include "texts/Parrots.txt"
};
const Image parrotsImg = { parrotsTxt, nullptr, 30, 5 };
#endif

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
#if SP_EDITION
	constexpr float timeOut = 12.f;
#else
	constexpr float timeOut = 4.f;
#endif
	if (AnyKeyJustPressed() || screenData.accumTime > timeOut)
	{
		nextState = GameStateId::running;
		screenData.accumTime = 0.f;
	}
	return (int)nextState;
}


void DisplayIntroScreen(Renderer& renderer, const void* data)
{
	const IntroScreenData& screenData = *(IntroScreenData*)data;

	renderer.DrawImage(GetImage(GetImageId(GameImageId::planet)), 0, 8, Color::greenIntense, ImageAlignment::centered, ImageAlignment::top);
#if SP_EDITION
	static const char* str[] =
	{
		"Social Point is under attack!",
		"Alien invaders want to steal Lua and Thor",
		"And use their DNA to create an army of super alien parrots",
		"to invade and conquer planet Earth.",
		"Avoid their lethal lasers",
		"and destroy them before they reach the building.",
		"Save the parrots and us all!"
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
	constexpr int numRows = std::size(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[0])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}

#if SP_EDITION
	renderer.DrawImage(parrotsImg, -20, 4, Color::yellowIntense, ImageAlignment::centered, ImageAlignment::bottom);
#endif
}
