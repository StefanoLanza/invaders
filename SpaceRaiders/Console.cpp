#include "stdafx.h"
#include "Console.h"
#include "DLL.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cassert>


bool InitConsole(Console& console, DLL& dll, const char* dllFileName)
{
	DLLError err = LoadDLL(dll, dllFileName);
	if (err != DLLError::ok)
	{
		return false;
	}

	console.handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console.handle)
	{
		return false;
	}

	err = GetDLLProcedure(&console.writeOutput, dll, "WriteConsoleOutput");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&console.resize, dll, "ResizeConsole");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&console.centerOnDesktop, dll, "CenterConsoleOnDesktop");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&console.showCursor, dll, "ShowConsoleCursor");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&console.hideCursor, dll, "HideConsoleCursor");
	if (err != DLLError::ok)
	{
		return false;
	}

	return true;
}
