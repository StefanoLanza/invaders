#pragma once

#include <cstddef>

#ifdef WINDOWS

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif


constexpr char DIR_SEPARATOR =
#if defined(_WIN32) || defined(WIN32)
    '\\';
#else
    '/';
#endif

void getExePath(char* path, size_t maxPathLen);
void cutPath(char* path);