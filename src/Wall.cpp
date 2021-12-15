#include "Images.h"
#include "Wall.h"
#include "Collision.h"


Wall NewWall(const Vector2D& pos, int maxHits)
{
	return {
		pos,
		{ ImageId::solidWall, Color::white },
		Wall::State::normal,
		maxHits };
}


Collider GetCollisionArea(Wall& wall)
{
	return { &wall, ColliderId::wall, wall.pos, wall.pos, GetImageSize(wall.visual.imageId), };
}


void HitWall(Wall& wall)
{
	wall.hits--;
	wall.visual.imageId = ImageId::brittleWall;
	if (wall.hits <= 0)
	{
		wall.state = Wall::State::dead;
	}
}
