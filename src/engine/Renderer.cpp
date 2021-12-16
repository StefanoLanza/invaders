#include "Renderer.h"

#include "MessageLog.h"
#include "Image.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <cmath>

#include "Console.h"

#ifdef WINDOWS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetMessage
#undef WriteConsoleOutput

#else

#ifdef __APPLE__
#include <ncurses.h>
#else
#include <ncursesw/curses.h>
#endif
struct CHAR_INFO {
	cchar_t ch;
};

#endif

namespace
{

#ifdef WINDOWS
constexpr WORD charColors[colorCount] =
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
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_BLUE | FOREGROUND_GREEN,
	FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_BLUE,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};
#else
constexpr attr_t attrs[colorCount] = {
	A_NORMAL,
	A_BOLD,
	A_NORMAL,
	A_BOLD,
	A_BOLD,
	A_NORMAL,
	A_BOLD,
	A_NORMAL,
	A_NORMAL,
	A_BOLD,
	A_NORMAL,
	A_BOLD,
	A_NORMAL,
	A_BOLD
};
#endif
}

constexpr int hudRows = 2; // rows reserved for the HUD


Renderer::Renderer() :
	consoleHandle {nullptr},
	bounds {0,0}
{

#ifdef WINDOWS
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(console.handle, &info);
#else
	initscr();
	start_color();
	curs_set(FALSE);
	cbreak();	
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);
	init_pair(6, COLOR_YELLOW, COLOR_BLACK);
	init_pair(7, COLOR_YELLOW, COLOR_BLACK);
	init_pair(8, COLOR_BLACK, COLOR_BLACK);
	init_pair(9, COLOR_WHITE, COLOR_BLACK);
	init_pair(10, COLOR_WHITE, COLOR_BLACK);
	init_pair(11, COLOR_BLUE, COLOR_BLACK);
	init_pair(12, COLOR_BLUE, COLOR_BLACK);
	init_pair(13, COLOR_BLUE, COLOR_BLACK);
	init_pair(14, COLOR_BLUE, COLOR_BLACK);
#endif
}


Renderer::~Renderer() {
#ifdef WINDOWS
#else
	endwin();
#endif
}


const IVector2D& Renderer::GetBounds() const
{
	return bounds;
}


bool Renderer::Initialize(int width, int height, int fontSize)
{
	assert(width > 0);
	assert(height > 0);
	bounds = { width, height };
	height += hudRows;
	canvas.resize(width * height);
#ifdef WINDOWS
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!consoleHandle)
	{
		return false;
	}
	bool r = ResizeConsole(consoleHandle, width, height, fontSize);
	r = r && CenterConsoleOnDesktop();
	return r;
#else
	consoleHandle = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
	if (! consoleHandle) {
		return false;
	}
	return true;
#endif
}

void Renderer::Clear(Color color)
{
	CHAR_INFO ch; 
#ifdef WINDOWS
	ch.Char.UnicodeChar = static_cast<WCHAR>(' ');
	ch.Attributes = charColors[(int)color];
#else
	setcchar(&ch.ch, L" ", attrs[(int)color], COLOR_PAIR((int)color), NULL);
#endif
	std::fill(canvas.begin(), canvas.end(), ch);
}


void Renderer::DrawCanvas()
{
#ifdef WINDOWS
	SMALL_RECT writeRegion = { 0, 0, (SHORT)bounds.x - 1, (SHORT)(bounds.y - 1 + hudRows) };
	WriteConsoleOutputW(consoleHandle, canvas.data(), { (SHORT)bounds.x, (SHORT)(bounds.y + hudRows) }, { 0, 0 }, &writeRegion);
#else
#endif
}


void Renderer::ClearLine(int row)
{
#ifdef WINDOWS
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	CHAR_INFO ch; 
	ch.Char.UnicodeChar = static_cast<WCHAR>(' ');
	ch.Attributes = charColors[(int)Color::black];
	for (int x = 0, ex = bounds.x; x != ex; ++x)
	{
		curCanvas[x] = ch;
	}
#endif
}


void Renderer::DisplayText(const char* str, int col, int row, Color color, ImageAlignment hAlignment)
{
	assert(str);
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	if (hAlignment == ImageAlignment::centered)
	{
		col = (bounds.x - (int)strlen(str)) / 2 + col;
	}
	for (int i = 0; str[i] != 0; ++i)
	{
		if (col >= 0 && col < bounds.x)
		{
#ifdef WINDOWS
			curCanvas[i + col].Char.UnicodeChar = static_cast<CHAR>(str[i]);
			curCanvas[i + col].Attributes = charColors[(int)color];
#else
#endif
		}
	}
}


void Renderer::DrawSprites(const RenderItem* sprites, int count)
{
	for (int i = 0; i < count; ++i)
	{
		const auto& ri = sprites[i];
		int x = (int)std::floor(ri.pos.x);
		int y = (int)std::floor(ri.pos.y);
		if (ri.visual.imageId != ImageId::null) {
			const Image& image = GetImage(ri.visual.imageId);
			x -= image.width / 2;
			y -= image.height / 2;
			if (image.colors)
			{
				DrawColoredImage(image, x, y);
			}
			else
			{
				DrawImage(image, x, y, ri.visual.color, ImageAlignment::left,  ImageAlignment::top);
			}
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


void Renderer::DrawImage(const Image& image, int x0, int y0, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment)
{
	CHAR_INFO* const dst = canvas.data();
	if (hAlignment == ImageAlignment::centered)
	{
		x0 = (bounds.x - image.width) / 2 + x0;
	}
	else if (hAlignment == ImageAlignment::right)
	{
		x0 = bounds.x - image.width - x0;
	}
	if (vAlignment == ImageAlignment::centered)
	{
		y0 = (bounds.y - image.height) / 2 + y0;
	}
	else if (vAlignment == ImageAlignment::bottom)
	{
		y0 = bounds.y- image.height - y0;
	}
	// Clip
	const int l = std::max(0, x0);
	const int r = std::min(bounds.x, x0 + image.width);
	const int b = std::max(0, y0);
	const int t = std::min(bounds.y, y0 + image.height);
#ifdef WINDOWS
	const WORD attrib = charColors[(int)color];

	for (int y = b; y < t; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x + (y + hudRows) * bounds.x];
			int s = (x - x0) + (y - y0) * (image.width + 1) + 1;  // +1: remove new lines, the first and at the end of each line
			if (image.img[s] != ' ')
			{
				c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
				c.Attributes = attrib;
			}
		}
	}
#else
#endif
}


void Renderer::DrawColoredImage(const Image& image, int x0, int y0)
{
	CHAR_INFO* const dst = canvas.data();
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
#ifdef WINDOWS
				c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
				c.Attributes = charColors[image.colors[s] - '0'];
#else
#endif
			}
		}
	}
}


void DrawImage(Renderer& renderer, const ImageA& image, int x0, int y0, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment)
{
	CHAR_INFO* const dst = renderer.canvas.data();
	IVector2D bounds = renderer.bounds;
	if (hAlignment == ImageAlignment::centered)
	{
		x0 = (bounds.x - image.width) / 2 + x0;
	}
	else if (hAlignment == ImageAlignment::right)
	{
		x0 = bounds.x - image.width - x0;
	}
	if (vAlignment == ImageAlignment::centered)
	{
		y0 = (bounds.y - image.height) / 2 + y0;
	}
	else if (vAlignment == ImageAlignment::bottom)
	{
		y0 = bounds.y- image.height - y0;
	}
	// Clip
	const int l = std::max(0, x0);
	const int r = std::min(bounds.x, x0 + image.width);
	const int b = std::max(0, y0);
	const int t = std::min(bounds.y, y0 + image.height);

#ifdef WINDOWS
	const WORD attrib = charColors[(int)color];

	for (int y = b; y < t; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x + (y + renderer.hudRows) * bounds.x];
			int s = (x - x0) + (y - y0) * (image.width);
			if (image.img[s] != ' ')
			{
				c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
				c.Attributes = attrib;
			}
		}
	}
#endif
}
