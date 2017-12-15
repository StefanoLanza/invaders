#include "stdafx.h"
#include "Images.h"
#include "Wall.h"
#include "Collision.h"


Wall::Wall(const Vector2D& pos, int maxHits) : 
	pos { pos },
	visual { ImageId::solidWall, Color::white },
	state { State::normal },
	hits { maxHits }
{}



const Vector2D& Wall::GetPosition() const
{
	return pos;
}


Collider Wall::GetCollisionArea()
{
	return { pos, pos, GetImageSize(visual.imageId), this, CollisionId::wall };
}

const Visual& Wall::GetVisual() const
{
	return visual;
}


Wall::State Wall::GetState() const
{
	return state;
}


void Wall::Hit()
{
	hits--;
	visual.imageId = ImageId::brittleWall;
	if (hits <= 0)
	{
		state = State::dead;
	}
}
