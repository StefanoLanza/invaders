#pragma once

#include <engine/Base.h>
#include <engine/Vector2D.h>

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

using ImageId = int;
constexpr ImageId nullImageId = 0;

void SetImage(ImageId imageId, const Image& image);
void SetImages(ImageId firstImageId, int imageCount, const Image* images);
const Image& GetImage(ImageId imageId);
Vector2D GetImageSize(ImageId imageId);
