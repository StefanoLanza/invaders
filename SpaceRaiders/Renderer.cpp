#include "stdafx.h"
#include <vector>
#include "Vector2D.h"
#include "Renderer.h"
#include "Game.h"
#include "MessageLog.h"
#include "Image.h"
#include <cassert>
#include <algorithm>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetMessage


// Images
const wchar_t ikaLogo[] =
{
	#include "IKA.txt"
};
const wchar_t raidersTxt[] =
{
	#include "raiders.txt"
};
const Image ikaImg = { ikaLogo, 25, 6 };
const Image raidersImg = { raidersTxt, 46, 6 };

namespace
{


constexpr WORD charColors[(int)Color::count] =
{
	FOREGROUND_RED,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN,
	FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_GREEN,
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	0,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
};


// Code taken and adapter from http://www.cplusplus.com/forum/windows/121444/
bool InitConsole(SHORT x, SHORT y, SHORT fontSize, HANDLE h)
{

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = fontSize;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(h, FALSE, &cfi);

	// If either dimension is greater than the largest console window we can have,
	// there is no point in attempting the change.
	const COORD largestSize = GetLargestConsoleWindowSize(h);
	x = std::min(x, largestSize.X);
	y = std::min(y, largestSize.Y);

	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	if (!GetConsoleScreenBufferInfo(h, &bufferInfo))
	{
		return false;
	}

    const SMALL_RECT& winInfo = bufferInfo.srWindow;
    const COORD windowSize = { winInfo.Right - winInfo.Left + 1, winInfo.Bottom - winInfo.Top + 1};

	if (windowSize.X > x || windowSize.Y > y)
	{
		// window size needs to be adjusted before the buffer size can be reduced.
		const SMALL_RECT info = 
		{ 
			0, 
			0, 
			x < windowSize.X ? x-1 : windowSize.X-1, 
			y < windowSize.Y ? y-1 : windowSize.Y-1 
		};

		if (!SetConsoleWindowInfo(h, TRUE, &info))
		{
			return false;
		}
	}

	if (!SetConsoleScreenBufferSize(h, { x, y } ))
	{
		return false;
	}

    const SMALL_RECT info = { 0, 0, x - 1, y - 1 };
    if (!SetConsoleWindowInfo(h, TRUE, &info))
	{
		return false;
	}

	return true;
}


}


const int Renderer::hudRows = 2; // rows reserved for the HUD


Renderer::Renderer(const IVector2D& bounds, int fontSize) :
	renderBounds {bounds},
	consoleHandle { GetStdHandle(STD_OUTPUT_HANDLE) }
{
	assert(bounds.x > 0);
	assert(bounds.y > 0);
	const int consoleWidth = bounds.x;
	const int consoleHeight = bounds.y + hudRows;
	const size_t canvasSize = consoleWidth * consoleHeight;
	canvas.resize(canvasSize);

	if (consoleHandle)
	{
		InitConsole((SHORT)consoleWidth, (SHORT)consoleHeight, (SHORT)fontSize, consoleHandle);
	}
}


Renderer::~Renderer() = default;


void Renderer::Update(const RenderItemList& sprites, const Game& game, const MessageLog& messageLog)
{
	if (consoleHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	// Hide cursor
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);

	FillCanvas(RaiderSprites::backgroundTile, Color::black);
	if (game.GetState() == Game::State::start)
	{
		DisplayStartMenu();
	}
	else if (game.GetState() == Game::State::intro)
	{
		DisplayIntro();
	}
	else if (game.GetState() == Game::State::paused)
	{
		DrawSprites(sprites);
		DisplayScores(game);
		DisplayPauseMenu();
	}
	else if (game.GetState() == Game::State::victory)
	{
		DrawSprites(sprites);
		DisplayScores(game);
		DisplayVictory();
	}
	else if (game.GetState() == Game::State::over)
	{
		DrawSprites(sprites);
		DisplayScores(game);
		DisplayGameOver();
	}
	else
	{
		// Running
		DrawSprites(sprites);
		DisplayScores(game);
	}
	DisplayMessages(messageLog);

	DrawCanvas();
}


void Renderer::DisplayScores(const Game& game)
{
	// Write scores
	char tmp[256];
	for (int p = 0; p < game.GetNumPlayers(); ++p)
	{
		sprintf_s(tmp, "P%d Score: %d", p + 1, game.GetScore(p));
		DisplayText(tmp, 0, p, Color::white);
	}
}


void Renderer::FillCanvas(RaiderSprites sprite, Color color)
{
	CHAR_INFO* curCanvas = canvas.data();
	CHAR_INFO ch; 
	ch.Char.UnicodeChar = static_cast<WCHAR>(sprite);
	ch.Attributes = charColors[(int)color];
	for (int i = 0, ei = (int)canvas.size(); i != ei; ++i)
	{
		curCanvas[i] = ch;
	}
}


void Renderer::DrawCanvas()
{
	const CHAR_INFO* curCanvas = canvas.data();
	SMALL_RECT writeRegion = { 0, 0, (SHORT)renderBounds.x - 1, (SHORT)(renderBounds.y - 1 + hudRows) };
	WriteConsoleOutputW(consoleHandle, curCanvas, COORD { (SHORT)renderBounds.x, (SHORT)(renderBounds.y + hudRows) }, COORD { 0, 0 }, &writeRegion);
}


void Renderer::ClearLine(int row)
{
	CHAR_INFO* curCanvas = canvas.data() + row * renderBounds.x;
	CHAR_INFO ch; 
	ch.Char.UnicodeChar = static_cast<WCHAR>(RaiderSprites::backgroundTile);
	ch.Attributes = charColors[(int)Color::black];
	for (int x = 0, ex = renderBounds.x; x != ex; ++x)
	{
		curCanvas[x] = ch;
	}
}


void Renderer::DisplayText(const char* str, int col, int row, Color color)
{
	CHAR_INFO* curCanvas = canvas.data() + row * renderBounds.x;
	for (int i = 0; str[i] != 0; ++i)
	{
		curCanvas[i + col].Char.UnicodeChar = static_cast<CHAR>(str[i]);
		curCanvas[i + col].Attributes = charColors[(int)color];
	}
}


void Renderer::DisplayStartMenu()
{
	DrawImage(ikaImg, 0, 0, Color::greenIntense, Alignment::centered);
	DrawImage(raidersImg, 0, 7, Color::greenIntense, Alignment::centered);

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
	constexpr int numRows = _countof(str);
	const int row = 16;
	const int col = (renderBounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		ClearLine(row + r);
		DisplayText(str[r], col, row + r, Color::white);
	}
}


void Renderer::DisplayIntro()
{
	static const char* str[] =
	{
		"",
		"Planet IKA is under attack!",
		"Destroy the alien invaders before they reach the ground",
		"Save the planet and earn eternal glory",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderBounds.y - numRows) / 2; // centered
	const int col = (renderBounds.x - (int)strlen(str[2])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		ClearLine(row + r);
		DisplayText(str[r], col, row + r, r == 0 ? Color::greenIntense : Color::white);
	}
}


void Renderer::DisplayPauseMenu()
{
	static const char* str[] =
	{
		"",
		"Press ENTER to continue game",
		"Press ESC to go back to main menu",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderBounds.y - numRows) / 2; // centered
	const int col = (renderBounds.x - (int)strlen(str[1])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		ClearLine(row + r);
		DisplayText(str[r], col, row + r, Color::white);
	}
}


void Renderer::DisplayVictory()
{
	static const char* str[] =
	{
		"",
		"You saved the planet!",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderBounds.y - numRows) / 2; // centered
	const int col = (renderBounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		ClearLine(row + r);
		DisplayText(str[r], col, row + r, Color::white);
	}
}


void Renderer::DisplayGameOver()
{
	static const char* str[] =
	{
		"",
		"Game Over",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderBounds.y - numRows) / 2; // centered
	const int col = (renderBounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		ClearLine(row + r);
		DisplayText(str[r], col, row + r, Color::white);
	}
}


void Renderer::DrawSprites(const RenderItemList& sprites)
{
	CHAR_INFO* const curCanvas = canvas.data();

	for (const auto& ri : sprites)
	{
		const int x = (int)std::floor(ri.pos.x);
		const int y = (int)std::floor(ri.pos.y);
		if (ri.image)
		{
			DrawImage(*ri.image, x, y, ri.color, Alignment::left);
		}
		else
		{
			if (x >= 0 && x < renderBounds.x && y >= 0 && y < renderBounds.y)
			{
				auto& c = curCanvas[x + (y + hudRows) * renderBounds.x];
				c.Char.UnicodeChar = static_cast<CHAR>(ri.sprite);
				c.Attributes = charColors[(int)ri.color];
			}
		}
	}
}


void Renderer::DisplayMessages(const MessageLog& messageLog)
{
	for (int i = 0; i < std::min(hudRows, messageLog.GetNumMessages()); ++i)
	{
		const auto msg = messageLog.GetMessage(i);
		const int x =  (renderBounds.x - (int)strlen(msg.first)) / 2; // centered
		DisplayText(msg.first, x, 0 + i, msg.second);
	}
}


void Renderer::DrawImage(const Image& image, int x0, int y0, Color color, Alignment alignment)
{
	CHAR_INFO* const dst = canvas.data();
	if (alignment == Alignment::centered)
	{
		x0 = (renderBounds.x - image.width) / 2 + x0;
	}
	else if (alignment == Alignment::right)
	{
		x0 = renderBounds.x - image.width + x0;
	}
	// Clip
	const int l = std::max(0, x0);
	const int r = std::min(renderBounds.x, x0 + image.width);
	const int b = std::max(0, y0);
	const int t = std::min(renderBounds.y, y0 + image.height);

	for (int y = b; y < t; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x + (y + hudRows) * renderBounds.x];
			int s = (x - x0) + (y - y0) * (image.width + 1) + 1;  // +1: remove new lines, the first and at the end of each line
			c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
			c.Attributes = charColors[(int)color];
		}
	}
}
