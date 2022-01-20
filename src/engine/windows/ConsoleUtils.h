#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void DisableCaption(HWND hwndWindow);
bool SetConsoleFontSize(HANDLE handle, int fontSize);
bool ResizeConsole(HANDLE handle, int cols, int rows);
bool CenterWindowOnDesktop(HWND hwndWindow);
bool IsWindowMinimized(HWND hwnd);
