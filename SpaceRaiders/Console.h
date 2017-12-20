#pragma once

#include "DLL.h"


struct Console
{
	void* handle = nullptr;
};


struct ConsoleModule
{
	using WriteOutput = void (*)(void* consoleHandle, const void* buff, int cols, int rows, int colOffs, int rowOffs);
	using CenterOnDesktop = bool (*)();
	using HideCursor = void (*)(void* consoleHandle);
	using ShowCursor = void (*)(void* consoleHandle);
	using Resize = bool (*) (void* consoleHandle, int cols, int rows, int fontSize);

	DLL             dll;
	// DLL entry points
	Resize          resize = nullptr;
	WriteOutput     writeOutput = nullptr;
	CenterOnDesktop centerOnDesktop = nullptr;
	HideCursor      hideCursor = nullptr;
	ShowCursor      showCursor = nullptr;
};


bool InitConsole(Console& console, ConsoleModule& module, const char* dllFileName);
