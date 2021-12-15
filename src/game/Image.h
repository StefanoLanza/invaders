#pragma once

#include <engine/Base.h>

struct Image
{
	const wchar_t* img; 
	const byte* colors;
	int width;
	int height;
};

// ASCII
struct ImageA
{
	const char* img; 
	int         width;
	int         height;
	bool        freeMemory;
};


bool LoadTxtImage(ImageA& image, int width, int height, const char* fileName);
