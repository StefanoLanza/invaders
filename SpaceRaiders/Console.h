#pragma once

#include "Colors.h"


struct Console
{
	using WriteOutput = void (*)(void* consoleHandle, const void* buff, int cols, int rows, int colOffs, int rowOffs);
	using CenterOnDesktop = bool (*)();
	using HideCursor = void (*)(void* consoleHandle);
	using ShowCursor = void (*)(void* consoleHandle);
	using Resize = bool (*) (void* consoleHandle, int cols, int rows, int fontSize);

	void*           handle = nullptr;
	// DLL entry points
	Resize          resize = nullptr;
	WriteOutput     writeOutput = nullptr;
	CenterOnDesktop centerOnDesktop = nullptr;
	HideCursor      hideCursor = nullptr;
	ShowCursor      showCursor = nullptr;
};

struct DLL;

bool InitConsole(Console& console, DLL& dll, const char* dllFileName);
