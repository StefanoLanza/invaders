#include "stdafx.h"
#include <vector>
#include "Vector2D.h"
#include "Renderer.h"
#include "Game.h"
#include "MessageLog.h"
#include "Image.h"
#include "Images.h"
#include <cassert>
#include <algorithm>
#include <iostream>
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
bool InitConsole(SHORT cols, SHORT rows, SHORT fontSize, HANDLE handle)
{

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = fontSize;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
	if (!SetCurrentConsoleFontEx(handle, TRUE, &cfi))
	{
		return false;
	}

	CONSOLE_SCREEN_BUFFER_INFOEX bufferInfo;
	bufferInfo.cbSize = sizeof(bufferInfo);
	if (!GetConsoleScreenBufferInfoEx(handle, &bufferInfo))
	{
		std::cout << "GetConsoleScreenBufferInfo error: " << GetLastError() << std::endl;
		return false;
	}

	const COORD largestSize = GetLargestConsoleWindowSize(handle);
	const COORD bufferSize = { cols, rows };

	BOOL success;

	bufferInfo.dwSize = bufferSize;
	bufferInfo.dwMaximumWindowSize  = bufferSize;
	if (!SetConsoleScreenBufferInfoEx(handle, &bufferInfo))
	{
		std::cout << "SetConsoleScreenBufferInfoEx error: " << GetLastError() << std::endl;
		return false;
	}
	SMALL_RECT newWindowRect = { 0, 0, cols - 1, rows - 1};
	if (!SetConsoleWindowInfo(handle, TRUE, &newWindowRect))
	{
		std::cout << "SetConsoleWindowInfo error: " << GetLastError() << std::endl;
		return false;
	}

	return true;
}


BOOL CenterWindow(HWND hwndWindow)
{
     HWND hwndParent;
     RECT rectWindow, rectParent;
 
     // make the window relative to its parent
     if ((hwndParent = GetAncestor(hwndWindow, GA_PARENT)) != NULL)
     {
         GetWindowRect(hwndWindow, &rectWindow);
         GetWindowRect(hwndParent, &rectParent);
 
         int nWidth = rectWindow.right - rectWindow.left;
         int nHeight = rectWindow.bottom - rectWindow.top;
 
         int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
         int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;
 
         int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
         int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
 
         // make sure that the dialog box never moves outside of the screen
         if (nX < 0) nX = 0;
         if (nY < 0) nY = 0;
         if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
         if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;
 
         MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);
 
         return TRUE;
     }
 
     return FALSE;
}


}


const int Renderer::hudRows = 2; // rows reserved for the HUD


Renderer::Renderer(const IVector2D& bounds) :
	bounds {bounds},
	consoleHandle { nullptr }
{
	assert(bounds.x > 0);
	assert(bounds.y > 0);
	const int consoleWidth = bounds.x;
	const int consoleHeight = bounds.y + hudRows;
	const size_t canvasSize = consoleWidth * consoleHeight;
	canvas.resize(canvasSize);
}


Renderer::~Renderer() = default;


bool Renderer::InitializeConsole(int fontSize)
{
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	const int consoleWidth = bounds.x;
	const int consoleHeight = bounds.y + hudRows;
	if (! consoleHandle)
	{
		return false;
	}
	if (! InitConsole((SHORT)consoleWidth, (SHORT)consoleHeight, (SHORT)fontSize, consoleHandle))
	{
		return false;
	}
	//if (!CenterWindow(GetConsoleWindow()))
	{
	//	return false;
	}
	return true;
}


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

	FillCanvas(Color::black);
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


void Renderer::FillCanvas(Color color)
{
	CHAR_INFO* curCanvas = canvas.data();
	CHAR_INFO ch; 
	ch.Char.UnicodeChar = static_cast<WCHAR>(' ');
	ch.Attributes = charColors[(int)color];
	for (int i = 0, ei = (int)canvas.size(); i != ei; ++i)
	{
		curCanvas[i] = ch;
	}
}


void Renderer::DrawCanvas()
{
	const CHAR_INFO* curCanvas = canvas.data();
	SMALL_RECT writeRegion = { 0, 0, (SHORT)bounds.x - 1, (SHORT)(bounds.y - 1 + hudRows) };
	WriteConsoleOutputW(consoleHandle, curCanvas, COORD { (SHORT)bounds.x, (SHORT)(bounds.y + hudRows) }, COORD { 0, 0 }, &writeRegion);
}


void Renderer::ClearLine(int row)
{
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	CHAR_INFO ch; 
	ch.Char.UnicodeChar = static_cast<WCHAR>(' ');
	ch.Attributes = charColors[(int)Color::black];
	for (int x = 0, ex = bounds.x; x != ex; ++x)
	{
		curCanvas[x] = ch;
	}
}


void Renderer::DisplayText(const char* str, int col, int row, Color color)
{
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	for (int i = 0; str[i] != 0; ++i)
	{
		curCanvas[i + col].Char.UnicodeChar = static_cast<CHAR>(str[i]);
		curCanvas[i + col].Attributes = charColors[(int)color];
	}
}


void Renderer::DisplayStartMenu()
{
	DrawImage(ikaImg, 0, 0, Color::greenIntense, Alignment::centered, Alignment::top);
	DrawImage(raidersImg, 0, 7, Color::greenIntense, Alignment::centered,  Alignment::top);

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
	const int col = (bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		ClearLine(row + r);
		DisplayText(str[r], col, row + r, Color::white);
	}
}


void Renderer::DisplayIntro()
{
	DrawImage(GetImage(ImageId::planet), 0, 8, Color::white, Alignment::centered,  Alignment::top);
	static const char* str[] =
	{
		"",
		"Planet IKA is under attack!",
		"Destroy the alien invaders before they reach the ground",
		"Save the planet and earn eternal glory",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[2])) / 2;
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
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[1])) / 2;
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
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[3])) / 2;
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
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		ClearLine(row + r);
		DisplayText(str[r], col, row + r, Color::white);
	}
}


void Renderer::DrawSprites(const RenderItemList& sprites)
{
	for (const auto& ri : sprites)
	{
		int x = (int)std::floor(ri.pos.x);
		int y = (int)std::floor(ri.pos.y);
		const Image& image = GetImage(ri.visual.imageId);
		if (image.img)
		{
			x -= image.width / 2;
			y -= image.height / 2;
			DrawImage(image, x, y, ri.visual.color, Alignment::left,  Alignment::top);
		}
	}
}


void Renderer::DisplayMessages(const MessageLog& messageLog)
{
	for (int i = 0; i < std::min(hudRows, messageLog.GetNumMessages()); ++i)
	{
		const auto msg = messageLog.GetMessage(i);
		const int x =  (bounds.x - (int)strlen(msg.first)) / 2; // centered
		DisplayText(msg.first, x, 0 + i, msg.second);
	}
}


void Renderer::DrawImage(const Image& image, int x0, int y0, Color color, Alignment hAlignment, Alignment vAlignment)
{
	CHAR_INFO* const dst = canvas.data();
	if (hAlignment == Alignment::centered)
	{
		x0 = (bounds.x - image.width) / 2 + x0;
	}
	else if (hAlignment == Alignment::right)
	{
		x0 = bounds.x - image.width + x0;
	}
	if (vAlignment == Alignment::centered)
	{
		y0 = (bounds.y - image.height) / 2 + y0;
	}
	else if (vAlignment == Alignment::bottom)
	{
		y0 = bounds.y- image.height + y0;
	}
	// Clip
	const int l = std::max(0, x0);
	const int r = std::min(bounds.x, x0 + image.width);
	const int b = std::max(0, y0);
	const int t = std::min(bounds.y, y0 + image.height);

	for (int y = b; y < t; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x + (y + hudRows) * bounds.x];
			int s = (x - x0) + (y - y0) * (image.width + 1) + 1;  // +1: remove new lines, the first and at the end of each line
			if (image.img[s] != ' ')
			{
				c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
				c.Attributes = charColors[(int)color];
			}
		}
	}
}
