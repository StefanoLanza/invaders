#include "Console.h"
#include "MessageLog.h"
#include "Image.h"
#include <cassert>
#include <cmath>
#include <cstring>


void Console::SetViewport(const Viewport& newViewport)
{
	assert(newViewport.right > newViewport.left);
	assert(newViewport.bottom > newViewport.top);
	viewport = newViewport;
	viewport.left = std::max(0, viewport.left);
	viewport.right = std::min(bounds.x, viewport.right);
	viewport.top = std::max(0, viewport.top);
	viewport.bottom = std::min(bounds.y, viewport.bottom);
}


void Console::SetDefaultViewport()
{
	viewport = { 0, 0, bounds.x, bounds.y };
}


const IVector2D& Console::GetBounds() const
{
	return bounds;
}

void Console::DrawSprites(const RenderItem* sprites, int count)
{
	for (int i = 0; i < count; ++i)
	{
		const auto& ri = sprites[i];
		int x = (int)std::floor(ri.pos.x);
		int y = (int)std::floor(ri.pos.y);
		if (ri.visual.imageId != nullImageId) {
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


void Console::DisplayMessages(const MessageLog& messageLog)
{
#if 0
	for (int i = 0; i < std::min(hudRows, messageLog.GetNumMessages()); ++i)
	{
		const auto msg = messageLog.GetMessage(i);
		const int x =  (bounds.x - (int)strlen(msg.first)) / 2; // centered
		DisplayText(msg.first, x, 0 + i, msg.second);
	}
#endif
}


void Console::DrawNumber(int number, int x, int y, const Image digitImages[10], Color color, TextAlignment alignment)
{
	int digits[16] = {};
	int digitCount = 0;
	constexpr int base = 10;
	do
	{
		digits[digitCount] = number % base;
		++digitCount;
		number /= base;
	} while (number > 0);

	constexpr int spacing = 1;
	int totalWidth = 0;
	if (alignment != TextAlignment::left) 
	{
		for (int i = 0; i < digitCount; ++i)
		{
			totalWidth += digitImages[digits[i]].width;
		}
		totalWidth += (digitCount - 1) * spacing;
	}
	if (alignment == TextAlignment::centered)
	{
		x -= totalWidth / 2;
	}
	else if (alignment == TextAlignment::right)
	{
		x -= totalWidth;
	}
	do 
	{ 
		int digit = digits[--digitCount];
		DrawImage(digitImages[digit], x, y, color, ImageAlignment::left, ImageAlignment::top);
		x += (digitImages[digit].width + spacing);
	} while (digitCount > 0);
}
