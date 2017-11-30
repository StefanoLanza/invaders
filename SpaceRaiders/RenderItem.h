#pragma once

#include "Vector2D.h"
#include "Colors.h"


enum class ImageId;

struct Visual
{
	ImageId       imageId;
	Color         color;
};

struct RenderItem
{
	Vector2D      pos;
	Visual        visual;
};
