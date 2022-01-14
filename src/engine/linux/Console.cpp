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
	A_NORMAL,
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

bool Console::IsMinimized() const
{
	return false;
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
	init_pair(6, COLOR_BLUE, COLOR_BLACK);
	init_pair(7, COLOR_YELLOW, COLOR_BLACK);
	init_pair(8, COLOR_YELLOW, COLOR_BLACK);
	init_pair(9, COLOR_BLACK, COLOR_BLACK);
	init_pair(10, COLOR_WHITE, COLOR_BLACK);
	init_pair(11, COLOR_WHITE, COLOR_BLACK);
	init_pair(12, COLOR_CYAN, COLOR_BLACK);
	init_pair(13, COLOR_CYAN, COLOR_BLACK);
	init_pair(14, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(15, COLOR_MAGENTA, COLOR_BLACK);
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


void Console::DisplayText(const char* str, int col, int row, Color color, TextAlignment alignment)
{
	assert(str);
	if (row < 0 || row >= bounds.y) {
		return;
	}
	const int colorPair = toColorPair(color);
	const attr_t attr = toAttr(color);
	CHAR_INFO* curCanvas = canvas.data() + row * bounds.x;
	if (alignment == TextAlignment::centered)
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

	CHAR_INFO* dst = canvas.data() + b * bounds.x;
	for (int y = b; y < t; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x];
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
		dst += bounds.x;
	}
}


void Console::DrawColoredImage(const Image& image, int x0, int y0)
{
	// Clip
	const int l = std::max(0, x0);
	const int r = std::min(bounds.x, x0 + image.width);
	const int b = std::max(0, y0);
	const int t = std::min(bounds.y, y0 + image.height);

	CHAR_INFO* dst = canvas.data() + b * bounds.x;
	for (int y = b; y < t; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			auto& c = dst[x];
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
		dst += bounds.x;
	}
}


extern const wchar_t consoleSymbols[];


void Console::DrawBorder(int x0, int y0, int width, int height, Color color)
{
	// Clip
	const int l = std::max(0, x0);
	const int r = std::min(bounds.x, x0 + width);
	const int t = std::max(0, y0);
	const int b = std::min(bounds.y, y0 + height);

	const attr_t attr = toAttr(color);
	const int colorPair = toColorPair(color);

	CHAR_INFO* const dst = canvas.data() + t * bounds.x;

	wchar_t chstr[2];
	chstr[1] = 0;
	if (y0 == t)
	{
		if (x0 == l)
		{
			chstr[0] = consoleSymbols[3];
			setcchar(&dst[l], chstr, attr, colorPair, NULL);
		}
		chstr[0] = consoleSymbols[4];
		for (int col = l + 1; col < r - 1; ++col)
		{
			setcchar(&dst[col], chstr, attr, colorPair, NULL);
		}
		if (x0 + width == r) 
		{
			chstr[0] = consoleSymbols[5];
			setcchar(&dst[r - 1], chstr, attr, colorPair, NULL);
		}
	}
	if (b == y0 + height)
	{
		const int offs = (b - t - 1) * bounds.x;
		if (x0 == l)
		{
			chstr[0] = consoleSymbols[7];
			setcchar(&dst[l + offs], chstr, attr, colorPair, NULL);
		}
		chstr[0] = consoleSymbols[4];
		for (int col = l + 1; col < r - 1; ++col)
		{
			setcchar(&dst[col + offs], chstr, attr, colorPair, NULL);
		}
		if (x0 + width == r) 
		{
			chstr[0] = consoleSymbols[8];
			setcchar(&dst[r - 1 + offs], chstr, attr, colorPair, NULL);
		}
	}

	if (l == x0)
	{
		chstr[0] = consoleSymbols[6];
		for (int row = 1; row < (b - t - 1); ++row)
		{
			setcchar(&dst[row * bounds.x + l], chstr, attr, colorPair, NULL);
		}
	}
	if (r == x0 + width)
	{
		chstr[0] = consoleSymbols[6];
		for (int row = 1; row < (b - t - 1); ++row)
		{
			setcchar(&dst[row * bounds.x + r - 1], chstr, attr, colorPair, NULL);
		}
	}
}


void Console::DrawRectangle(int x0, int y0, int width, int height, Color color)
{
	// Clip
	const int l = std::max(0, x0);
	const int r = std::min(bounds.x, x0 + width);
	const int t = std::max(0, y0);
	const int b = std::min(bounds.y, y0 + height);

	wchar_t chstr[2];
	chstr[0] = consoleSymbols[1];
	chstr[1] = 0;

	const attr_t attr = toAttr(color);
	const int colorPair = toColorPair(color);

	CHAR_INFO* dst = canvas.data() + t * bounds.x;
	for (int y = t; y < b; ++y)
	{
		for (int x = l; x < r; ++x)
		{
			setcchar(&dst[x], chstr, attr, colorPair, NULL);
		}
		dst += bounds.x;
	}
}
