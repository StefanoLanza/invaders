#include "StartMenu.h"
#include <engine/Base.h>
#include "GameStates.h"
#include "Game.h"
#include <engine/Input.h>
#include <engine/Console.h>
#include "Images.h"
#include <engine/MessageLog.h>
#include "PlayField.h"
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
const Image ikaImg = { ikaLogo, nullptr, 18, 5 };
const Image raidersImg = { raidersTxt, nullptr, 46, 6 };
const Image invadersImg = { invadersTxt, nullptr, 45, 5 };
}


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
	const uint maxFlakes = 100;
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
//		for (int i = 0; i < 20; ++i)
		{
			CreateSnowFlake(data, world);
		}
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
		if (sf.pos.y >= world.bounds.y - 0.5f)
		{
//			sf.pos.y = world.bounds.y - 0.5f;
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
}


int StartMenu(Game& game, void* data_, float dt)
{
	StartMenuData& data = *(StartMenuData*)data_;
	data.t += dt;
#if XMAS_EDITION
	AnimateFlakes(data, game.world, dt);
#endif
	GameStateId newState;
	game.numPlayers = 0;
	game.messageLog.Clear();
	game.world.DestroyAll(); // FIXME OnEnter

	if (KeyJustPressed(KeyCode::_1))
	{
		game.mode = Game::Mode::p1;
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_2))
	{
		game.mode = Game::Mode::cpu1;
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_3))
	{
		game.mode = Game::Mode::p1p2;
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_4))
	{
		game.mode = Game::Mode::p1cpu2;
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_5))
	{
		game.mode = Game::Mode::cpu1cpu2;
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::quit;
	}
	else
	{
		newState = GameStateId::start;
	}
	return (int)newState;
}


void DisplayStartMenu(Console& renderer, const void* data_)
{
#if XMAS_EDITION
	const StartMenuData& data = *(const StartMenuData*)data_;
	const Image& snowFlakeImage = GetImage(GetImageId(GameImageId::snowFlake));
	for (const auto& sf : data.snowFlakes)
	{
		renderer.DrawImage(snowFlakeImage, (int)std::floor(sf.pos.x), (int)std::floor(sf.pos.y), Color::whiteIntense, ImageAlignment::left, ImageAlignment::top);
	}
#endif

	renderer.DrawImage(ikaImg, 0, 4, Color::yellowIntense, ImageAlignment::centered, ImageAlignment::top);
	renderer.DrawImage(invadersImg, 0, 11, Color::yellowIntense, ImageAlignment::centered,  ImageAlignment::top);

#if XMAS_EDITION
	const bool blink = std::sin(20.f * data.t) > 0.f;
	renderer.DisplayText("Christmas Edition", 0, 20, blink ? Color::redIntense : Color::red, ImageAlignment::centered);
#endif
	static const char* str[] =
	{
		"",
		"Single Player",
		"  Press 1 to start one player game",
		"  Press 2 to start one CPU game",
		"",
		"Multi Player",
		"  Press 3 to start two players game",
		"  Press 4 to start player and CPU game",
		"  Press 5 to start two CPUs game",
		"",
		"Press ESC to quit"
	};
	constexpr int row = 22;
	constexpr int numRows = static_cast<int>(std::size(str));
	const int col = (renderer.GetBounds().x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		//renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}

#if XMAS_EDITION
	renderer.DrawImage(GetImage(GetImageId(GameImageId::gift)), 4, 0, Color::whiteIntense, ImageAlignment::left, ImageAlignment::bottom);
	renderer.DrawImage(GetImage(GetImageId(GameImageId::happyHolidays)), 0, 8, blink ? Color::redIntense : Color::red, ImageAlignment::centered, ImageAlignment::bottom);
	renderer.DrawImage(GetImage(GetImageId(GameImageId::xmasLeaf)), 12, 4, Color::greenIntense, ImageAlignment::right, ImageAlignment::top);
#endif
}

