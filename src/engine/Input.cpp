#include "Input.h"
#include "Keyboard.h"
#include <cstring>

RndInput::RndInput(std::default_random_engine& rGen) : 
	rGen {rGen}, 
	rndInt { { 30, 30, 30 }},
	accumTime {0},
	state {0}
{}


void RndInput::Update(float dt)
{
	accumTime += dt;
	if (accumTime > 0.25f)
	{
		accumTime = 0.f;
		state =	rndInt(rGen);
	}
}


bool RndInput::Left() const
{ 
	return state == 0;
}

bool RndInput::Right() const
{ 
	return state == 2; 
}

bool RndInput::Fire() const {
	return true;
}


KeyboardInput::KeyboardInput(KeyCode left, KeyCode right, KeyCode fire) : 
	left { left },
	right { right },
	fire { fire }
{}


void KeyboardInput::Update(float /*dt*/)
{}


bool KeyboardInput::Left() const
{ 
	return KeyPressed(left);
}


bool KeyboardInput::Right() const
{ 
	return KeyPressed(right);
}

bool KeyboardInput::Fire() const {
	return KeyPressed(fire);
}
