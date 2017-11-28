#pragma once

#include <vector>
#include "Vector2D.h"
#include "RenderItem.h"


typedef struct _CHAR_INFO CHAR_INFO; // forward declaration of C struct
typedef std::vector<RenderItem> RenderItemList;
class Game;
class MessageLog;
struct Image;


class Renderer
{
public:

	Renderer(const IVector2D& bounds, int fontSize);
	~Renderer();

	// Display all game objects, score, game over message
	void Update(const RenderItemList& sprites, const Game& game, const MessageLog& messageLog);

private:

	enum class Alignment
	{
		left,
		centered, 
		right
	};

	// Fills whole canvas array with sprite
	void FillCanvas(RaiderSprites sprite, Color color);

	// Prints canvas char array on console
	void DrawCanvas();

	void ClearLine(int row);
	void DisplayText(const char* str, int col, int row, Color color);
	void DisplayStartMenu();
	void DisplayIntro();
	void DisplayPauseMenu();
	void DisplayVictory();
	void DisplayGameOver();
	void DisplayScores(const Game& game);
	void DrawSprites(const RenderItemList& sprites);
	void DisplayMessages(const MessageLog& messageLog);
	void DrawImage(const Image& image, int x, int y, Color color, Alignment alignment);

private:

	static const int hudRows;

	IVector2D renderBounds;
	// Double buffering is not necessary when using WriteConsoleOutput
	std::vector<CHAR_INFO> canvas;
	void* consoleHandle;
};

