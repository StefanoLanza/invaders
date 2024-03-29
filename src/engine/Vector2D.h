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
Vector2D Rotate(const Vector2D& v, float cos, float sin);
Vector2D Rotate(const Vector2D& v, float angle);
float Length(const Vector2D& v);
float SquareLength(const Vector2D& v);
Vector2D ComputeClosestNormal(const Vector2D& v);


// Integer version for the console size
struct IVector2D
{
	int x;
	int y;
};


constexpr float pi = 3.14159265358979323846f;
constexpr float two_pi = 2.f * pi;
