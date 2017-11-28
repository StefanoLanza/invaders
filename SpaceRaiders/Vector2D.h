#pragma once

// Vector2D is used to store x,y coordinates for game objects and to represent the world bounds. 
struct Vector2D
{
	float x;
	float y;
};


// TODO Add operators

Vector2D Lerp(const Vector2D& v0, const Vector2D& v1, float t);
// return v0 + v1 * t
Vector2D Mad(const Vector2D& v0, const Vector2D& v1, float t);
Vector2D Add(const Vector2D& v0, const Vector2D& v1);
// Return v0 - v1
Vector2D Sub(const Vector2D& v0, const Vector2D& v1);
Vector2D Mul(const Vector2D& v, float s);
float Dot(const Vector2D& v0, const Vector2D& v1);
Vector2D Reflect(const Vector2D& v, const Vector2D& n);
Vector2D Normalize(const Vector2D& v, float l = 1.f);
float Length(const Vector2D& v);
Vector2D ComputeClosestNormal(const Vector2D& v);


// Integer version for the console size
struct IVector2D
{
	int x;
	int y;
};


// Collision area is a rectangle defined by the current and previous position, plus a fixed radius of 0.5
// OK for this game, not for more accurate collisions and oblique trajectories. In that case we'd need collision routines
// for circles, swept circles, rectangles, rays etc
struct CollisionArea
{
	Vector2D v0;
	Vector2D v1;
	float    radius;
};


bool Intersect(const CollisionArea& a, const CollisionArea& b);