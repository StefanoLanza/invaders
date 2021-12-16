#pragma once

#include <vector>
#include "Vector2D.h"
#include "RenderItem.h"

#ifdef WINDOWS
typedef struct _CHAR_INFO CHAR_INFO; // forward declaration of C struct
#else
struct CHAR_INFO;
#endif

using RenderItemList = std::vector<RenderItem>;
class MessageLog;
struct Image;
struct ImageA;


enum class ImageAlignment
{
	left,
	centered,
	right,
	top,
	bottom
};


class Renderer
{
public:

	Renderer();
	~Renderer();

	const IVector2D& GetBounds() const;
	bool Initialize(int width, int height, int fontSize);
	void Clear(Color color);
	// Prints canvas char array on console
	void DrawCanvas();
	void ClearLine(int row);
	void DisplayText(const char* str, int col, int row, Color color, ImageAlignment hAlignment = ImageAlignment::left);
	void DisplayMessages(const MessageLog& messageLog);
	void DrawImage(const Image& image, int x, int y, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment);
	void DrawColoredImage(const Image& image, int x, int y);
	void DrawSprites(const RenderItem* sprites, int count);

public:

	void* consoleHandle;
	IVector2D bounds;
	std::vector<CHAR_INFO> canvas;
};


void DrawImage(Renderer& renderer, const ImageA& image, int x, int y, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment);
