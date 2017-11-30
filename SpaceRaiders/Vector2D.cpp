#include "stdafx.h"
#include "Vector2D.h"
#include <algorithm>
#include <cassert>


Vector2D Lerp(const Vector2D& v0, const Vector2D& v1, float t)
{
	assert(t >= 0.f && t <= 1.f);
	return { v0.x * (1.f - t) + v1.x * t, v0.y * (1.f - t) + v1.y * t };
}

Vector2D Mad(const Vector2D& v0, const Vector2D& v1, float t)
{
	return { v0.x + v1.x * t, v0.y + v1.y * t };
}

Vector2D Add(const Vector2D& v0, const Vector2D& v1)
{
	return { v0.x + v1.x, v0.y + v1.y };
}

Vector2D Sub(const Vector2D& v0, const Vector2D& v1)
{
	return { v0.x - v1.x, v0.y - v1.y };
}

Vector2D Mul(const Vector2D& v, float s)
{
	return { v.x * s, v.y * s };
}


float Dot(const Vector2D& v0, const Vector2D& v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}


Vector2D Reflect(const Vector2D& v, const Vector2D& n)
{
	const float d = Dot(v, n);
	return { v.x - 2.f * n.x * d, v.y - 2.f * n.y * d };
}

Vector2D Normalize(const Vector2D& v, float nl)
{
	assert(nl > 0.f);
	const float l = std::max(1e-8f, Length(v));
	return { v.x * (nl / l), v.y * (nl / l) };
}

float Length(const Vector2D& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

