#include "StartMenu.h"
#include <engine/Base.h>
#include "GameStates.h"
#include "Game.h"
#include "GameStateMgr.h"
#include "Images.h"
#include "PlayField.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include <engine/MessageLog.h>
#include "Images.h"
#include <cassert>
#include <cstring>


namespace
{
// Images
const wchar_t ikaLogo[] =
{
	#include "texts/IKA.txt"
};
const wchar_t raidersTxt[] =
{
	#include "texts/raiders.txt"
};
const wchar_t invadersTxt[] =
{
	#include "texts/invaders.txt"
};
const Image ikaImg = { ikaLogo, nullptr, 20, 4 };
const Image raidersImg = { raidersTxt, nullptr, 46, 6 };
const Image invadersImg = { invadersTxt, nullptr, 48, 4 };


#if XMAS_EDITION
struct SnowFlake
{
	Vector2D pos;
	float    dir;
	float    t;
};
#endif

struct StartMenuData
{
#if XMAS_EDITION
	std::vector<SnowFlake> snowFlakes;
	uint snowFlakeIndex = 0;
	float lastSnowX;
	float snow_t = 0;
#endif
	int   selection = 0;
	float t = 0;
};
StartMenuData startMenuData;

#if XMAS_EDITION
void CreateSnowFlake(StartMenuData& data, PlayField& world)
{
	float x;
	do
	{
		x = 8.f + world.rndFloat01(world.rGen) * (world.bounds.x - 16.f);
	} while (std::abs(x - data.lastSnowX) < 10.f);
	data.lastSnowX = x;
	float dx = -2.f + 4.f * world.rndFloat01(world.rGen);
	SnowFlake sf = { {x,0.f}, dx, 0.f };
	constexpr uint maxFlakes = 100;
	if (data.snowFlakes.size() > maxFlakes)
	{
		data.snowFlakes[data.snowFlakeIndex] = sf;
		data.snowFlakeIndex = (data.snowFlakeIndex + 1) % maxFlakes;
	}
	else
	{
		data.snowFlakes.push_back(sf);
	}
}

void AnimateFlakes(StartMenuData& data, PlayField& world, float dt)
{
	data.snow_t += dt;
	static float interval = 0.25f;
	if (data.snowFlakes.empty())
	{
		data.lastSnowX = 0.f;
		CreateSnowFlake(data, world);
	}
	if (data.snow_t > interval)
	{
		CreateSnowFlake(data, world);
		data.snow_t = 0;
	}
	for (auto& sf : data.snowFlakes)
	{
		//sf.x
		sf.pos.x += dt * sf.dir;
		sf.pos.y += 3.0f * dt;
		sf.t += dt;
		if (sf.t > 2.f)
		{
			sf.dir = -sf.dir;
			sf.t = 0.f;
		}
	}
}
#endif


void EnterStartMenu(void* data_, Game& game, int currentState)
{
	StartMenuData& data = *(StartMenuData*)data_;
#if XMAS_EDITION
	data.snowFlakes.clear();
	data.snowFlakeIndex = 0;
	data.snow_t = 0.f;
#endif
	data.t = 0.f;
	data.selection = 0;
}


int StartMenu(Game& game, void* data_, float dt)
{
	StartMenuData& data = *(StartMenuData*)data_;
	data.t += dt;
#if XMAS_EDITION
	AnimateFlakes(data, game.world, dt);
#endif
	GameStateId newState = GameStateId::start;
	game.numPlayers = 0;
	game.messageLog.Clear();

	if (KeyJustPressed(KeyCode::up))
	{
		data.selection = (data.selection - 1 + 3) % 3;
	}
	else if (KeyJustPressed(KeyCode::down))
	{
		data.selection = (data.selection + 1) % 3;
	}
	if (KeyJustPressed(KeyCode::enter))
	{
		if (data.selection == 0)
		{
			game.mode = Game::Mode::p1;
			newState = GameStateId::play;
		}
		else if (data.selection == 1)
		{
			game.mode = Game::Mode::p1p2;
			newState = GameStateId::play;
		}
		else
		{
			newState = GameStateId::quit;
		}
	}
	else if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::quit;
	}
	return (int)newState;
}


void DisplayStartMenu(Console& console, const void* data_)
{
	const StartMenuData& data = *(const StartMenuData*)data_;
#if XMAS_EDITION
	const Image& snowFlakeImage = GetImage(GameImageId::snowFlake);
	for (const auto& sf : data.snowFlakes)
	{
		console.DrawImage(snowFlakeImage, (int)std::floor(sf.pos.x), (int)std::floor(sf.pos.y), Color::whiteIntense, ImageAlignment::left, ImageAlignment::top);
	}
#endif

	const float blink = std::sin(20.f * data.t);
	const Color logoColors[4] = 
	{
		Color::yellowIntense,
		Color::yellow,
		Color::red,
		Color::redIntense
	};
	const int logoColorIdx = std::clamp((int)(+2.f + blink * 2.f), 0, 3);
	console.DrawImage(ikaImg, 0, 7, logoColors[logoColorIdx], ImageAlignment::centered, ImageAlignment::top);
	console.DrawImage(invadersImg, 0, 14, logoColors[logoColorIdx], ImageAlignment::centered,  ImageAlignment::top);

#if XMAS_EDITION
	console.DisplayText("Christmas Edition", 0, 20, blink > 0.f ? Color::redIntense : Color::red, TextAlignment::centered);
#endif

#if 1
	static const char* str[] =
	{
		"ONE PLAYER GAME",
		"TWO PLAYERS GAME",
		"QUIT"
	};
	const int textCol = 70;
	const int textRow = 30;
	for (int r = 0; r < (int)std::size(str); ++r)
	{
		console.DisplayText(str[r], textCol, textRow + r * 2, Color::white);
	}
	console.DisplayText(">", textCol - 3, textRow + data.selection * 2, Color::white);

#else
	const int textCol = 50;
	const int textRow = 30;
	console.DrawImage(GetImage(GameImageId::cursor), textCol, textRow + data.selection * 6, Color::white, ImageAlignment::left, ImageAlignment::top);
	console.DrawImage(GetImage(GameImageId::press1), textCol + 8, textRow, Color::lightBlue, ImageAlignment::left, ImageAlignment::top);
	console.DrawImage(GetImage(GameImageId::press2), textCol + 8, textRow + 6, Color::lightBlue, ImageAlignment::left, ImageAlignment::top);
	console.DrawImage(GetImage(GameImageId::pressESC), textCol + 8, textRow + 12, Color::lightBlue, ImageAlignment::left, ImageAlignment::top);
#endif
#if XMAS_EDITION
	console.DrawImage(GetImage(GameImageId::gift), 4, 0, Color::whiteIntense, ImageAlignment::left, ImageAlignment::bottom);
	console.DrawImage(GetImage(GameImageId::happyHolidays), 30, 8, blink ? Color::redIntense : Color::red, ImageAlignment::centered, ImageAlignment::bottom);
	console.DrawImage(GetImage(GameImageId::xmasLeaf), 12, 4, Color::greenIntense, ImageAlignment::right, ImageAlignment::top);
#endif
}


}

void RegisterStartMenu(Game& game)
{
	RegisterGameState(game, &startMenuData, StartMenu, DisplayStartMenu, EnterStartMenu, nullptr);	
}

