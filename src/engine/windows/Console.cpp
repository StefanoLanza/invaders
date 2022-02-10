#include "../Console.h"
#include "../Image.h"
#include "ConsoleUtils.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <cmath>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetMessage
#undef WriteConsoleOutput

namespace
{

constexpr WORD charColors[colorCount] =
{
	FOREGROUND_RED,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN,
	FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_BLUE,
	FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_GREEN,
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	0,
	FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_BLUE | FOREGROUND_GREEN,
	FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_BLUE,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};

}


Console::Console() :
	consoleHandle {nullptr},
	bounds {0,0},
	viewport {}
{
}


Console::~Console() = default;


bool Console::IsMinimized() const
{
	return IsWindowMinimized(GetConsoleWindow());
}


bool Console::Initialize(int width, int height, int fontSize)
{
	assert(width > 0);
	assert(height > 0);
	bounds = { width, height };
	canvas.resize(width * height);
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!consoleHandle)
	{
		return false;
	}

	// Disable user selection
	// ref: https://stackoverflow.com/questions/46567248/how-to-disable-user-selection-in-windows-console
	const HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(inputHandle, ENABLE_EXTENDED_FLAGS);

	const HWND hwnd = GetConsoleWindow();
	DisableCaption(hwnd);

	SetConsoleFontSize(consoleHandle, fontSize);

	if (!ResizeConsole(consoleHandle, width, height))
	{
		return false;
	}
	CenterWindowOnDesktop(hwnd);

	CONSOLE_CURSOR_INFO info {};
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);

	SetDefaultViewport();

	return true;
}

void Console::Clear(Color color)
{
	CHAR_INFO ch; 
	ch.Char.UnicodeChar = static_cast<WCHAR>(' ');
	ch.Attributes = charColors[(int)color];
	std::fill(canvas.begin(), canvas.end(), ch);
}


void Console::DrawCanvas()
{
	SMALL_RECT writeRegion = { 0, 0, (SHORT)bounds.x - 1, (SHORT)(bounds.y - 1) };
	WriteConsoleOutputW(consoleHandle, canvas.data(), { (SHORT)bounds.x, (SHORT)(bounds.y) }, { 0, 0 }, &writeRegion);
}


void Console::ClearLine(int row)
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


void Console::DisplayText(const char* str, int col, int row, Color color, TextAlignment alignment)
{
	assert(str);
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	if (alignment == TextAlignment::centered)
	{
		col = (bounds.x - (int)strlen(str)) / 2 + col;
	}
	else if (alignment == TextAlignment::right)
	{
		col = (bounds.x - 1 - (int)strlen(str)) + col;
	}
	for (int i = 0; str[i] != 0; ++i)
	{
		if (col >= bounds.x)
		{
			break;
		}
		if (col >= 0)
		{
			curCanvas[i + col].Char.UnicodeChar = static_cast<WCHAR>(str[i]);
			curCanvas[i + col].Attributes = charColors[(int)color];
		}
	}
}


void Console::DrawImage(const Image& image, int x0, int y0, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment)
{
	CHAR_INFO* const dst = canvas.data();
	if (hAlignment == ImageAlignment::centered)
	{
		x0 = (viewport.right - viewport.left - image.width) / 2 + x0 + viewport.left;
	}
	else if (hAlignment == ImageAlignment::right)
	{
		x0 = viewport.right - image.width - x0 + viewport.left;
	}
	else
	{
		x0 += viewport.left;
	}
	if (vAlignment == ImageAlignment::centered)
	{
		y0 = (viewport.bottom - viewport.top - image.height) / 2 + y0 + viewport.top;
	}
	else if (vAlignment == ImageAlignment::bottom)
	{
		y0 = viewport.bottom - image.height - y0 + viewport.top;
	}
	else
	{
		y0 += viewport.top;
	}
	// Clip
	const int l = std::max(viewport.left, x0);
	const int r = std::min(viewport.right, x0 + image.width);
	const int t = std::max(viewport.top, y0);
	const int b = std::min(viewport.bottom, y0 + image.height);
	if (l >= r)
	{
		return;
	}
	const WORD attrib = charColors[(int)color];

	for (int y = t; y < b; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x + y * bounds.x];
			int s = (x - x0) + (y - y0) * (image.width + 1) + 1;  // +1: remove new lines, the first and at the end of each line
			if (image.img[s] != ' ')
			{
				c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
				c.Attributes = attrib;
			}
		}
	}
}


void Console::DrawColoredImage(const Image& image, int x0, int y0)
{
	CHAR_INFO* const dst = canvas.data();
	x0 += viewport.left;
	y0 += viewport.top;
	// Clip
	const int l = std::max(viewport.left, x0);
	const int r = std::min(viewport.right, x0 + image.width);
	const int t = std::max(viewport.top, y0);
	const int b = std::min(viewport.bottom, y0 + image.height);
	if (l >= r)
	{
		return;
	}
	for (int y = t; y < b; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x + y * bounds.x];
			int s = (x - x0) + (y - y0) * (image.width + 1) + 1;  // +1: remove new lines, the first and at the end of each line
			if (image.img[s] != ' ')
			{
				c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
				c.Attributes = charColors[image.colors[s] - '0'];
			}
		}
	}
}


void Console::DrawChar(wchar_t ch, int x, int y, Color color)
{
	if (x < viewport.left || x >= viewport.right || y < viewport.top || y >= viewport.bottom)
	{
		return;
	}
	CHAR_INFO* dst = canvas.data() + x + y * bounds.x;
	dst->Char.UnicodeChar = static_cast<WCHAR>(ch);
	dst->Attributes = charColors[(int)color];
}

extern const wchar_t consoleSymbols[];


void Console::DrawBorder(int x0, int y0, int width, int height, Color color)
{
	// Clip
	const int l = std::max(viewport.left, x0);
	const int r = std::min(viewport.right, x0 + width);
	const int t = std::max(viewport.top, y0);
	const int b = std::min(viewport.bottom, y0 + height);


	CHAR_INFO* dst = canvas.data() + t * bounds.x;
	CHAR_INFO ch; 
	ch.Attributes = charColors[(int)color];
	
	if (y0 == t)
	{
		if (x0 == l)
		{
			ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[3]);
			dst[l] = ch;
		}
		ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[4]);
		for (int col = l + 1; col < r - 1; ++col)
		{
			dst[col] = ch;
		}
		if (x0 + width == r) 
		{
			ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[5]);
			dst[r - 1] = ch;
		}
	}
	if (b == y0 + height)
	{
		const int offs = (b - t - 1) * bounds.x;
		if (x0 == l)
		{
			ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[7]);
			dst[l + offs] = ch;
		}
		ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[4]);
		for (int col = l + 1; col < r - 1; ++col)
		{
			dst[col + offs] = ch;
		}
		if (x0 + width == r) 
		{
			ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[8]);
			dst[r - 1 + offs] = ch;
		}
	}
	if (l == x0)
	{
		ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[6]);
		for (int row = 1; row < (b - t - 1); ++row)
		{
			dst[row * bounds.x + l] = ch;
		}
	}
	if (r == x0 + width)
	{
		ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[6]);
		for (int row = 1; row < (b - t - 1); ++row)
		{
			dst[row * bounds.x + r - 1] = ch;
		}
	}
}


void Console::DrawRectangle(int x0, int y0, int width, int height, Color color)
{
	CHAR_INFO* const dst = canvas.data();

	// Clip
	const int l = std::max(viewport.left, x0);
	const int r = std::min(viewport.right, x0 + width);
	const int t = std::max(viewport.top, y0);
	const int b = std::min(viewport.bottom, y0 + height);

	CHAR_INFO ch; 
	ch.Char.UnicodeChar = static_cast<WCHAR>(consoleSymbols[1]);
	ch.Attributes = charColors[(int)color];
	for (int y = t; y < b; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			dst[x + y * bounds.x] = ch;
		}
	}
}