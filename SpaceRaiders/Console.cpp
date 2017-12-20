#include "Console.h"
#include "DLL.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cassert>


bool InitConsole(Console& console, ConsoleModule& module, const char* dllFileName)
{
	DLLError err = LoadDLL(module.dll, dllFileName);
	if (err != DLLError::ok)
	{
		return false;
	}

	console.handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console.handle)
	{
		return false;
	}

	err = GetDLLProcedure(&module.writeOutput, module.dll, "WriteConsoleOutput");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&module.resize, module.dll, "ResizeConsole");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&module.centerOnDesktop, module.dll, "CenterConsoleOnDesktop");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&module.showCursor, module.dll, "ShowConsoleCursor");
	if (err != DLLError::ok)
	{
		return false;
	}
	err = GetDLLProcedure(&module.hideCursor, module.dll, "HideConsoleCursor");
	if (err != DLLError::ok)
	{
		return false;
	}

	return true;
}
