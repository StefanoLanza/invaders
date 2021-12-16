#include "Path.h"
#include <cstring>

void getExePath(char* path, size_t maxPathLen) {
#ifdef WINDOWS
	// Get the path and the name
	GetModuleFileNameA( GetModuleHandle(NULL), path, maxPathLen);
#endif
}

void cutPath(char* path)
{
    // get '/' or '\\' depending on unix/mac or windows.
	int e = (int)strlen(path) - 1;
	while (e >= 0)
	{
		if (path[e] == DIR_SEPARATOR)
		{
			path[e] = 0;
			return;
		}
		e--;
	}
}
