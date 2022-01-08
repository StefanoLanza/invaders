#include "../Console.h"

#include "../Image.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <locale.h>
#include <ncurses.h>

namespace
{

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


int toColorPair(Color color) {
	return (int)color + 1;
}

attr_t toAttr(Color color) {
	return attrs[(int)color];
}

}

Console::Console() :
	consoleHandle {nullptr},
	bounds {0,0}
{
}


Console::~Console() {
	endwin();
}


bool Console::Initialize(int width, int height, int fontSize)
{
	assert(width > 0);
	assert(height > 0);
	height += hudRows;
	bounds = { width, height };
	initscr();
	consoleHandle = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
	if (! consoleHandle) {
		endwin();
		return false;
	}
	canvas.resize(width * height);
	setlocale(LC_ALL, "");
	start_color();
	curs_set(FALSE);
	cbreak();	
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	noecho();
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
	init_pair(13, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(14, COLOR_MAGENTA, COLOR_BLACK);
	return true;
}

void Console::Clear(Color color)
{
	CHAR_INFO ch; 
	setcchar(&ch, L" ", attrs[(int)color], toColorPair(color), NULL);
	std::fill(canvas.begin(), canvas.end(), ch);
}


void Console::DrawCanvas()
{
	WINDOW* win = static_cast<WINDOW*>(consoleHandle);

	const cchar_t* s = canvas.data();
	for (int y = 0; y < bounds.y + hudRows; ++y) {
		mvwadd_wchnstr(win, y, 0, s, bounds.x);
		s += bounds.x;
	}

	wrefresh(win);
}


void Console::ClearLine(int row)
{
	CHAR_INFO ch; 
	setcchar(&ch, L" ", attrs[(int)Color::black], toColorPair(Color::black), NULL);
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	for (int x = 0, ex = bounds.x; x != ex; ++x)
	{
		curCanvas[x] = ch;
	}
}


void Console::DisplayText(const char* str, int col, int row, Color color, ImageAlignment hAlignment)
{
	assert(str);
	if (row < 0 || row >= bounds.y) {
		return;
	}
	const int colorPair = toColorPair(color);
	const attr_t attr = toAttr(color);
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	if (hAlignment == ImageAlignment::centered)
	{
		col = (bounds.x - (int)strlen(str)) / 2 + col;
	}
	for (int i = 0; str[i] != 0; ++i)
	{
		if (col >= 0 && col < bounds.x)
		{
			wchar_t chstr[2];
			chstr[0] = str[i];
			chstr[1] = 0;
			int err = setcchar(&curCanvas[i + col], chstr, attr, colorPair, NULL);
			(void)err;
			assert(err == OK);
		}
	}
}


void Console::DrawImage(const Image& image, int x0, int y0, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment)
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
	const attr_t attr = toAttr(color);
	const int colorPair = toColorPair(color);

	for (int y = b; y < t; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x + y * bounds.x];
			int s = (x - x0) + (y - y0) * (image.width + 1) + 1;  // +1: remove new lines, the first and at the end of each line
			if (image.img[s] != ' ')
			{
				wchar_t chstr[2];
				chstr[0] = image.img[s];
				chstr[1] = 0;
				int err = setcchar(&c, chstr, attr, colorPair, NULL);
				(void)err;
				assert(err == OK);
			}
		}
	}
}


void Console::DrawColoredImage(const Image& image, int x0, int y0)
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
			auto& c = dst[x + y * bounds.x];
			int s = (x - x0) + (y - y0) * (image.width + 1) + 1;  // +1: remove new lines, the first and at the end of each line
			if (image.img[s] != ' ')
			{
				Color color = static_cast<Color>(image.colors[s] - '0');
				wchar_t chstr[2];
				chstr[0] = image.img[s];
				chstr[1] = 0;
				int err = setcchar(&c, chstr, toAttr(color), toColorPair(color), NULL);
				(void)err;
				assert(err == OK);
			}
		}
	}
}
