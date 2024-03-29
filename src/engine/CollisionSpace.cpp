#include "CollisionSpace.h"
#include <algorithm>


namespace {

constexpr uint64 MakeBit(ColliderId id0, ColliderId id1)
{
	uint64_t bit = (uint64_t)id0 * (uint64_t)ColliderId::count + (uint64_t)id1;
	return (uint64)1u << bit;
}

}


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

void CollisionSpace::Execute(const CollisionCallbackInfo callbackInfo[], int numCallbacks, void* context) const
{
	uint64_t collisionMask = 0;
	for (int i = 0; i < numCallbacks; ++i) {
		collisionMask |= MakeBit(callbackInfo[i].id0, callbackInfo[i].id1);
	}

	const int nc = (int)colliderData.size();
	const ColliderData* c0 = colliderData.data();
	const Rectangle* r0 = rectangles.data();

	for (int x = 0; x < nc - 1; ++x, ++c0, ++r0)
	{
		const ColliderData* c1 = c0 + 1;
		const Rectangle* r1 = r0 + 1;
		for (int y = 0; y < nc - x - 1; ++y, ++c1, ++r1)
		{
			const uint64_t pairIdx = MakeBit(c0->id, c1->id) | MakeBit(c1->id, c0->id);
			if ((collisionMask & pairIdx) && Intersect(*r0, *r1))
			{
				// Callback
				for (int i = 0; i < numCallbacks; ++i) {
					const CollisionCallbackInfo& cbk = callbackInfo[i];
					if (c0->id == cbk.id0 && c1->id == cbk.id1) {
						cbk.fnc(context, c0->userData, c1->userData);
						break;
					}
					if (c1->id == cbk.id0 && c0->id == cbk.id1) {
						cbk.fnc(context, c1->userData, c0->userData);
						break;
					}
				}
			}
		}
	}
}


bool CollisionSpace::TestRect(const Rectangle& rect, ColliderId colliderId) const
{
	const int nc = (int)colliderData.size();
	const ColliderData* c0 = colliderData.data();
	const Rectangle* r0 = rectangles.data();
	for (int c = 0; c < nc; ++c, ++c0, ++r0)
	{
		if (c0->id == colliderId && Intersect(rect, *r0))
		{
			return true;
		}
	}
	return false;
}
