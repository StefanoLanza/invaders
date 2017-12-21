#pragma once

#include <vector>
#include "Vector2D.h"
#include "RenderItem.h"


typedef struct _CHAR_INFO CHAR_INFO; // forward declaration of C struct
typedef std::vector<RenderItem> RenderItemList;
class Game;
class MessageLog;
struct Image;
struct Console;
struct ConsoleModule;


class Renderer
{
public:

	Renderer(const IVector2D& bounds, Console& console, ConsoleModule& consoleModule);
	~Renderer();

	const IVector2D& GetBounds() const;
	bool InitializeConsole(int fontSize);
	// Display all game objects, score, game over message
	void Update(const RenderItemList& sprites, const Game& game, const MessageLog& messageLog);

	enum class Alignment
	{
		left,
		centered, 
		right,
		top,
		bottom
	};

	// Fills whole canvas array with sprite
	void FillCanvas(Color color);

	// Prints canvas char array on console
	void DrawCanvas();

	void ClearLine(int row);
	void DisplayText(const char* str, int col, int row, Color color);
	void DisplayMessages(const MessageLog& messageLog);
	void DrawImage(const Image& image, int x, int y, Color color, Alignment hAlignment, Alignment vAlignment);
	void DisplayScores(const Game& game);
	void DrawSprites(const RenderItem* sprites, int count);

public:

	static const int hudRows;

	Console& console;
	ConsoleModule& consoleModule;
	IVector2D bounds;
	std::vector<CHAR_INFO> canvas;
};
