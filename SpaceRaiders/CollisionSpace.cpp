#include "stdafx.h"
#include "CollisionSpace.h"


CollisionSpace::CollisionSpace() = default;


void CollisionSpace::Add(const Collider& collider)
{
	colliders.push_back(collider);
}


void CollisionSpace::Clear()
{
	colliders.clear();
}


int CollisionSpace::Execute(CollisionInfo collisionInfo[], int maxCollisions) const
{
	// FIXME O(n2)
	const int nc = (int)colliders.size();
	int nci = 0;
	const Collider* c0 = colliders.data();
	for (int x = 0; x < nc - 1; ++x, ++c0)
	{
		const Collider* c1 = c0 + 1;
		for (int y = 0; y < nc - x - 1; ++y, ++c1)
		{
			// FIXME Collision matrix
			if (c0->id != c1->id && Intersect(*c0, *c1))
			{
				if (c0->id <= c1->id)
				{
					collisionInfo[nci] = { c0->userData, c1->userData, c0->id, c1->id };
				}
				else
				{
					collisionInfo[nci] = { c1->userData, c0->userData, c1->id, c0->id };
				}
				nci++;
				if (nci >= maxCollisions)
				{
					return nci;
				}
			}
		}
	}
	return nci;
}
