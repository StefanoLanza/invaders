#pragma once

#include "Collision.h"
#include <vector>


using CollisionCallback = void (*) (void* context, void* ud0, void* ud1);

struct CollisionCallbackInfo
{
	ColliderId id0;
	ColliderId id1;
	CollisionCallback fnc;
};

class CollisionSpace
{
public:

	CollisionSpace();

	void Add(const Collider& collider);
	void Clear();
	void Execute(const CollisionCallbackInfo callbackInfo[], int numCallbacks, void* context) const;
	bool TestRect(const Rectangle& rect, ColliderId colliderId) const;

private:

	std::vector<Rectangle>    rectangles;
	std::vector<ColliderData> colliderData;
};
