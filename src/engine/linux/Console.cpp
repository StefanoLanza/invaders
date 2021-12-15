#include "../Base.h"
#include "../Console.h"
#include <ncurses.h>
#include <cstring>
#include <cstdlib>
#include <iostream>


bool InitConsole(Console& console)
{
	console.handle = nullptr;
	if (!console.handle)
	{
		return false;
	}
	return true;
}


void WriteConsoleOutput(void* consoleHandle, const void* buff, int cols, int rows, int colOffs, int rowOffs)
{
	if (consoleHandle)
	{
	}
}


bool CenterConsoleOnDesktop()
{
	return false;
}


void HideConsoleCursor(void* handle)
{
}


void ShowConsoleCursor(void* handle)
{
}

bool ResizeConsole(void* handle, int cols, int rows, int fontSize)
{
	return false;
}
