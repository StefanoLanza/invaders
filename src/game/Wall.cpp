#include "Images.h"
#include "Wall.h"
#include <engine/Collision.h>


Wall NewWall(const Vector2D& pos, int maxHits)
{
	return {
		pos,
		{ GameImageId::solidWall, Color::white },
		Wall::State::normal,
		maxHits };
}


Collider GetCollider(Wall& wall)
{
	return { &wall, ColliderId::wall, wall.pos, wall.pos, GetImageSize(wall.visual.imageId), };
}


void HitWall(Wall& wall)
{
	wall.hits--;
	wall.visual.imageId = GameImageId::brittleWall;
	if (wall.hits <= 0)
	{
		wall.state = Wall::State::dead;
	}
}
