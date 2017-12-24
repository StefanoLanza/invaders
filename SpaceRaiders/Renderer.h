#pragma once

#include <vector>
#include "Vector2D.h"
#include "RenderItem.h"


typedef struct _CHAR_INFO CHAR_INFO; // forward declaration of C struct
typedef std::vector<RenderItem> RenderItemList;
struct Game;
class MessageLog;
struct Image;
struct ImageA;
struct Console;


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

	Renderer(const IVector2D& bounds, Console& console);
	~Renderer();

	const IVector2D& GetBounds() const;
	bool InitializeConsole(int fontSize);
	// Display all game objects, score, game over message
	void Update(const RenderItemList& sprites, const Game& game, const MessageLog& messageLog);

	// Fills whole canvas array with sprite
	void FillCanvas(Color color);

	// Prints canvas char array on console
	void DrawCanvas();

	void ClearLine(int row);
	void DisplayText(const char* str, int col, int row, Color color, ImageAlignment hAlignment = ImageAlignment::left);
	void DisplayMessages(const MessageLog& messageLog);
	void DrawImage(const Image& image, int x, int y, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment);
	void DrawColoredImage(const Image& image, int x, int y);
	void DisplayScores(const Game& game);
	void DrawSprites(const RenderItem* sprites, int count);

public:

	static const int hudRows;

	Console& console;
	IVector2D bounds;
	std::vector<CHAR_INFO> canvas;
};


void DrawImage(Renderer& renderer, const ImageA& image, int x, int y, Color color, ImageAlignment hAlignment, ImageAlignment vAlignment);
