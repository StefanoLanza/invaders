#pragma once

enum class Color
{
	red,
	redIntense,
	green,
	greenIntense,
	blue,
	yellow,
	yellowIntense,
	black,
	white,
	whiteIntense,
	lightBlue,
	lightBlueIntense,
	violet,
	violetIntense
};

constexpr int colorCount = static_cast<int>(Color::violetIntense) + 1;
