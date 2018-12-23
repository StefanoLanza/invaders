#pragma once


struct Console
{
	void* handle = nullptr;
};


bool InitConsole(Console& console);
void WriteConsoleOutput(void* consoleHandle, const void* buff, int cols, int rows, int colOffs, int rowOffs);
bool CenterConsoleOnDesktop();
void HideConsoleCursor(void* consoleHandle);
void ShowConsoleCursor(void* consoleHandle);
bool ResizeConsole(void* consoleHandle, int cols, int rows, int fontSize);
