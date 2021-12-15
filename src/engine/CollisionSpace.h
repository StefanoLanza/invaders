#pragma once

#include "Base.h"
#include "Collision.h"
#include <vector>


class CollisionSpace
{
public:

	CollisionSpace();

	void Add(const Collider& collider);
	void Clear();
	int Execute(CollisionInfo collisionInfo[], int maxCollisions) const;

private:

	std::vector<Rectangle>    rectangles;
	std::vector<ColliderData> colliderData;
};
