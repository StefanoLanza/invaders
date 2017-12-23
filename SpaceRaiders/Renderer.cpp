#include "Renderer.h"
#include "Game.h"
#include "MessageLog.h"
#include "Image.h"
#include "Images.h"
#include "Console.h"
#include "GameStates.h"
#include <cassert>
#include <algorithm>
#include <iostream>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetMessage
#undef WriteConsoleOutput


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
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_BLUE | FOREGROUND_GREEN,
	FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_BLUE,
	FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};


}


const int Renderer::hudRows = 2; // rows reserved for the HUD


Renderer::Renderer(const IVector2D& bounds_, Console& console_, ConsoleModule& consoleModule_) :
	console {console_},
	consoleModule { consoleModule_ },
	bounds {bounds_}
{
	assert(bounds.x > 0);
	assert(bounds.y > 0);
	const int consoleWidth = bounds.x;
	const int consoleHeight = bounds.y + hudRows;
	const size_t canvasSize = consoleWidth * consoleHeight;
	canvas.resize(canvasSize);
}


Renderer::~Renderer() = default;


const IVector2D& Renderer::GetBounds() const
{
	return bounds;
}


bool Renderer::InitializeConsole(int fontSize)
{
	const int consoleWidth = bounds.x;
	const int consoleHeight = bounds.y + hudRows;
	bool r = consoleModule.resize(console.handle, consoleWidth, consoleHeight, fontSize);
	r = r && consoleModule.centerOnDesktop();
	return r;
}


void Renderer::Update(const RenderItemList& sprites, const Game& game, const MessageLog& messageLog)
{
	FillCanvas(Color::black);
	DrawSprites(sprites.data(), (int)sprites.size());
	DisplayScores(game);
	DisplayMessages(messageLog);
	DrawGameState(game, *this);
	DrawCanvas();
}


void Renderer::DisplayScores(const Game& game)
{
	// Write scores
	char tmp[256];
	for (int p = 0; p < game.numPlayers; ++p)
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
	consoleModule.writeOutput(console.handle, canvas.data(), bounds.x, bounds.y, 0, hudRows);
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


void Renderer::DrawSprites(const RenderItem* sprites, int count)
{
	for (int i = 0; i < count; ++i)
	{
		const auto& ri = sprites[i];
		int x = (int)std::floor(ri.pos.x);
		int y = (int)std::floor(ri.pos.y);
		const Image& image = GetImage(ri.visual.imageId);
		if (image.img)
		{
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
				c.Char.UnicodeChar = static_cast<WCHAR>(image.img[s]);
				c.Attributes = charColors[image.colors[s] - '0'];
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
}
