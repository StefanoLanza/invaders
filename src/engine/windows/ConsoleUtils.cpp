#include "../Base.h"
#include "../Console.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <iostream>


namespace
{

bool CenterWindowOnDesktop(HWND hwndWindow);
bool ResizeConsoleImpl(SHORT cols, SHORT rows, SHORT fontSize, HANDLE handle);


bool CenterWindowOnDesktop(HWND hwndWindow)
{
	// make the window relative to its parent
	if (HWND hwndParent = GetDesktopWindow(); hwndParent != NULL) //  GetAncestor(hwndWindow, GA_PARENT)) != NULL)
	{
		RECT rectWindow, rectParent;
		GetWindowRect(hwndWindow, &rectWindow);
		GetWindowRect(hwndParent, &rectParent);
 
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
 
		SetWindowPos(hwndWindow, nullptr, nX, nY, nWidth, nHeight, SWP_SHOWWINDOW || SWP_NOSIZE);
 
		return true;
	}

	return false;
}

// Code taken and adapter from http://www.cplusplus.com/forum/windows/121444/
bool ResizeConsoleImpl(SHORT cols, SHORT rows, SHORT fontSize, HANDLE handle)
{
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = fontSize;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
	if (!SetCurrentConsoleFontEx(handle, TRUE, &cfi))
	{
		return false;
	}

	CONSOLE_SCREEN_BUFFER_INFOEX bufferInfo;
	bufferInfo.cbSize = sizeof(bufferInfo);
	if (!GetConsoleScreenBufferInfoEx(handle, &bufferInfo))
	{
		std::cerr << "GetConsoleScreenBufferInfo error: " << GetLastError() << std::endl;
		return false;
	}

	const COORD largestSize = GetLargestConsoleWindowSize(handle);
	const COORD bufferSize = { cols, rows };

	bufferInfo.dwSize = bufferSize;
	bufferInfo.dwMaximumWindowSize  = bufferSize;
	if (!SetConsoleScreenBufferInfoEx(handle, &bufferInfo))
	{
		std::cerr << "SetConsoleScreenBufferInfoEx error: " << GetLastError() << std::endl;
		return false;
	}
	SMALL_RECT newWindowRect = { 0, 0, cols - 1, rows - 1};
	if (!SetConsoleWindowInfo(handle, TRUE, &newWindowRect))
	{
		std::cerr << "SetConsoleWindowInfo error: " << GetLastError() << std::endl;
		return false;
	}

	return true;
}


}

bool ResizeConsole(void* handle, int cols, int rows, int fontSize)
{
	return ResizeConsoleImpl(static_cast<SHORT>(cols), static_cast<SHORT>(rows), static_cast<SHORT>(fontSize), handle);
}

bool CenterConsoleOnDesktop()
{
	return CenterWindowOnDesktop(GetConsoleWindow());
}
