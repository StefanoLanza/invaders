#include "ConsoleUtils.h"
#include "../Console.h"
#include <cstring>
#include <cstdlib>
#include <iostream>


void DisableCaption(HWND hwndWindow)
{
	const auto style = GetWindowLong(hwndWindow, GWL_STYLE);
	SetWindowLong(hwndWindow, GWL_STYLE, style & ~WS_CAPTION);
}


bool IsWindowMinimized(HWND hwnd) 
{
	LONG styles = GetWindowLong(hwnd, GWL_STYLE);
	return styles & WS_MINIMIZE; 
}

bool SetConsoleFontSize(HANDLE handle, int fontSize)
{
	CONSOLE_FONT_INFOEX cfi {};
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y =  static_cast<SHORT>(fontSize);                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
	if (SetCurrentConsoleFontEx(handle, TRUE, &cfi))
	{
		return true;
	}
	return false;
}

// Code taken and adapter from http://www.cplusplus.com/forum/windows/121444/
bool ResizeConsole(HANDLE handle, int cols, int rows)
{
	const COORD largestSize = GetLargestConsoleWindowSize(handle);
	if (cols > largestSize.X || rows > largestSize.Y) 
	{
		std::cerr << "Invalid console size" << std::endl;
		return false;
	}

	CONSOLE_SCREEN_BUFFER_INFOEX bufferInfo {};
	bufferInfo.cbSize = sizeof(bufferInfo);
	if (!GetConsoleScreenBufferInfoEx(handle, &bufferInfo))
	{
		std::cerr << "GetConsoleScreenBufferInfo error: " << GetLastError() << std::endl;
		return false;
	}

	const COORD bufferSize = {  static_cast<SHORT>(cols),  static_cast<SHORT>(rows) };
	bufferInfo.dwSize = bufferSize;
	bufferInfo.dwMaximumWindowSize  = bufferSize;
	if (!SetConsoleScreenBufferInfoEx(handle, &bufferInfo))
	{
		std::cerr << "SetConsoleScreenBufferInfoEx error: " << GetLastError() << std::endl;
		return false;
	}

	const SMALL_RECT newWindowRect = { 0, 0,  static_cast<SHORT>(cols - 1),  static_cast<SHORT>(rows - 1) };
	if (!SetConsoleWindowInfo(handle, TRUE, &newWindowRect))
	{
		std::cerr << "SetConsoleWindowInfo error: " << GetLastError() << std::endl;
		return false;
	}

	return true;
}


bool CenterWindowOnDesktop(HWND hWnd)
{
	// Needed ? Sometimes the window is not centered vertically  
	SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	// make the window relative to its parent
	if (HWND hwndParent = GetDesktopWindow(); hwndParent != NULL) //  GetAncestor(hwndWindow, GA_PARENT)) != NULL)
	{
		RECT rectWindow, rectParent;
		if (FALSE == GetWindowRect(hWnd, &rectWindow))
		{
			return false;
		}
		if (FALSE == GetWindowRect(hwndParent, &rectParent))
		{
			return false;
		}
 
		int nWidth = rectWindow.right - rectWindow.left;
		int nHeight = rectWindow.bottom - rectWindow.top;
 
		int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;
 
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
 
		// make sure that the dialog box never moves outside of the screen
		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
		if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;
 
		SetWindowPos(hWnd, nullptr, nX, nY, nWidth, nHeight, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOACTIVATE);
 
		return true;
	}

	return false;
}
