#pragma once

#include "Collision.h"
#include <cstddef>
#include <vector>


using CollisionCallback = void (*) (void*, void*, void*);

struct CollisionCallbackInfo
{
	ColliderId id0;
	ColliderId id1;
	CollisionCallback fnc;
	void* userData;
};

constexpr uint64 MakeBit(ColliderId id0, ColliderId id1)
{
	uint64_t bit = (uint64_t)id0 * (uint64_t)ColliderId::count + (uint64_t)id1;
	return (uint64)1u << bit;
}

class CollisionSpace
{
public:

	CollisionSpace();

	void Add(const Collider& collider);
	void Clear();
	int Execute(CollisionInfo collisionInfo[], int maxCollisions, uint64_t collisionMask) const;
	void Execute(const CollisionCallbackInfo callbackInfo[], int numCallbacks) const;

private:

	std::vector<Rectangle>    rectangles;
	std::vector<ColliderData> colliderData;
};
