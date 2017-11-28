#include "stdafx.h"
#include "Wall.h"


Wall::Wall(const Vector2D& pos, int maxHits) : 
	pos { pos },
	visual { pos, RaiderSprites::wallSolid, Color::white },
	state { State::normal },
	hits { maxHits }
{}



const Vector2D& Wall::GetPosition() const
{
	return pos;
}


CollisionArea Wall::GetCollisionArea() const
{
	return { pos, pos, 0.5f };
}

const RenderItem& Wall::GetVisual() const
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
	visual.sprite =  RaiderSprites::wallBrittle;
	if (hits <= 0)
	{
		state = State::dead;
	}
}
