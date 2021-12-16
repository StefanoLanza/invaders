#pragma once

#include "Vector2D.h"
#include "Colors.h"
#include "Image.h"


struct Visual
{
	ImageId       imageId;
	Color         color;
};

struct Animation
{
	static constexpr int numFrames = 2;
	ImageId  images[numFrames];
	float    duration; // [s]
};

struct AnimState
{
	int frame = 0;
	float t = 0.f;
};


struct RenderItem
{
	Vector2D      pos;
	Visual        visual;
};


void UpdateAnimation(AnimState& state, const Animation& anim, float dt);
