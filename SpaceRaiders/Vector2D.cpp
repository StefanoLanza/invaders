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


Vector2D ComputeClosestNormal(const Vector2D& v)
{
	float avx = std::abs(v.x);
	float avy = std::abs(v.y);
	Vector2D n;
	if (avx > avy)
	{
		if (v.x > 0.f)
		{
			n = { -1.f, 0.f }; 
		}
		else
		{
			n =	{ +1.f, 0.f };
		}
	}
	else
	{
		if (v.y > 0.f)
		{
			n = { 0, -1 };
		}
		else
		{
			n = { 0, +1 };
		}
	}
	return n;
}


bool Intersect(const CollisionArea& a, const CollisionArea& b)
{
	Vector2D aMin = { std::min(a.v0.x, a.v1.x), std::min(a.v0.y, a.v1.y) };
	Vector2D aMax = { std::max(a.v0.x, a.v1.x), std::max(a.v0.y, a.v1.y) };
	Vector2D bMin = { std::min(b.v0.x, b.v1.x), std::min(b.v0.y, b.v1.y) };
	Vector2D bMax = { std::max(b.v0.x, b.v1.x), std::max(b.v0.y, b.v1.y) };
	// 
	const float radius = 0.5f;
	aMin.x -= a.radius;
	aMax.x += a.radius;
	bMin.x -= b.radius;
	bMax.x += b.radius;
	aMin.y -= a.radius;
	aMax.y += a.radius;
	bMin.y -= b.radius;
	bMax.y += b.radius;
	// Poor-man continuous collision detection
	// Here we assume a collision area that includes the current and previous position of a game object
	return (aMax.x >= bMin.x && bMax.x >= aMin.x && aMax.y >= bMin.y && bMax.y >= aMin.y);
}
