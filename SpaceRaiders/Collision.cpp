#include "stdafx.h"
#include "Collision.h"
#include <algorithm>
#include <cassert>


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


/*
CollisionInfo RectVSRect(const Rectangle& a, const Rectangle& b)
{
	Vector2D aMin = { std::min(a.v0.x, a.v1.x), std::min(a.v0.y, a.v1.y) };
	Vector2D aMax = { std::max(a.v0.x, a.v1.x), std::max(a.v0.y, a.v1.y) };
	Vector2D bMin = { std::min(b.v0.x, b.v1.x), std::min(b.v0.y, b.v1.y) };
	Vector2D bMax = { std::max(b.v0.x, b.v1.x), std::max(b.v0.y, b.v1.y) };
	CollisionInfo ci;
	ci.valid = (aMax.x >= bMin.x && bMax.x >= aMin.x && aMax.y >= bMin.y && bMax.y >= aMin.y);
	return ci;
}*/


bool Intersect(const Collider& a, const Collider& b)
{
	Vector2D aMin = { std::min(a.v0.x, a.v1.x), std::min(a.v0.y, a.v1.y) };
	Vector2D aMax = { std::max(a.v0.x, a.v1.x), std::max(a.v0.y, a.v1.y) };
	Vector2D bMin = { std::min(b.v0.x, b.v1.x), std::min(b.v0.y, b.v1.y) };
	Vector2D bMax = { std::max(b.v0.x, b.v1.x), std::max(b.v0.y, b.v1.y) };
	aMin.x -= a.size.x * 0.5f;
	aMax.x += a.size.x * 0.5f;
	bMin.x -= b.size.x * 0.5f;
	bMax.x += b.size.x * 0.5f;
	aMin.y -= a.size.y * 0.5f;
	aMax.y += a.size.y * 0.5f;;
	bMin.y -= b.size.y * 0.5f;;
	bMax.y += b.size.y * 0.5f;;
	// Poor-man continuous collision detection
	// Here we assume a collision area that includes the current and previous position of a game object
	return (aMax.x >= bMin.x && bMax.x >= aMin.x && aMax.y >= bMin.y && bMax.y >= aMin.y);
}
