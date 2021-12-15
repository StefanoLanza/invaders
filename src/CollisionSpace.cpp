#include "CollisionSpace.h"
#include <algorithm>


CollisionSpace::CollisionSpace() = default;


void CollisionSpace::Add(const Collider& collider)
{
	Rectangle r;
	r.v0 = { std::min(collider.v0.x, collider.v1.x), std::min(collider.v0.y, collider.v1.y) };
	r.v1 = { std::max(collider.v0.x, collider.v1.x), std::max(collider.v0.y, collider.v1.y) };
	r.v0.x -= collider.size.x * 0.5f;
	r.v1.x += collider.size.x * 0.5f;
	r.v0.y -= collider.size.y * 0.5f;
	r.v1.y += collider.size.y * 0.5f;

	colliderData.push_back(collider.data);
	rectangles.push_back(r);
}


void CollisionSpace::Clear()
{
	colliderData.clear();
	rectangles.clear();
}


inline uint64 MakeBit(ColliderId id0, ColliderId id1)
{
	uint bit = (uint)id0 * (uint)ColliderId::count + (uint)id1;
	return (uint64)1u << bit;
}


int CollisionSpace::Execute(CollisionInfo collisionInfo[], int maxCollisions) const
{
	const int nc = (int)colliderData.size();
	int nci = 0;
	const ColliderData* c0 = colliderData.data();
	const Rectangle* r0 = rectangles.data();

	uint64 collisionMask = 0;
	collisionMask |= MakeBit(ColliderId::player,  ColliderId::alien);
	collisionMask |= MakeBit(ColliderId::player,  ColliderId::alienLaser);
	collisionMask |= MakeBit(ColliderId::player,  ColliderId::powerUp);
	collisionMask |= MakeBit(ColliderId::alien,  ColliderId::playerLaser);
	collisionMask |= MakeBit(ColliderId::alien,  ColliderId::wall);
	collisionMask |= MakeBit(ColliderId::playerLaser,  ColliderId::wall);
	collisionMask |= MakeBit(ColliderId::alienLaser,  ColliderId::playerLaser);

	for (int x = 0; x < nc - 1; ++x, ++c0, ++r0)
	{
		const ColliderData* c1 = c0 + 1;
		const Rectangle* r1 = r0 + 1;
		for (int y = 0; y < nc - x - 1; ++y, ++c1, ++r1)
		{
			const uint64 pairIdx = MakeBit(c0->id, c1->id) | MakeBit(c1->id, c0->id);
			if ((collisionMask & pairIdx) && c0->userData != c1->userData && Intersect(*r0, *r1))
			{
				collisionInfo[nci] = { c0->userData, c1->userData, c0->id, c1->id };
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
