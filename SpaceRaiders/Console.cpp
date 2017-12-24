#include "Console.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cassert>


bool InitConsole(Console& console)
{
	console.handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console.handle)
	{
		return false;
	}
	return true;
}
