#include "Input.h"

#include <cstring>

#ifdef WINDOWS
#include <windows.h>
#else
#include <ncurses.h>
#endif

namespace
{


int prevKeyState[numKeyCodes] = { -1 };
int keyState[numKeyCodes] = { -1 };

#ifdef WINDOWS
const int keyCodeToVKey[numKeyCodes] =
{
	VK_ESCAPE,
	VK_RETURN,
	VK_SPACE,
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	'1',
	'2',
	'3',
	'4',
	'5',
	'A',
	'D'
};
#else
const int keyCodeToVKey[numKeyCodes] =
{
	27, //ESCAPE,
	KEY_ENTER,
	KEY_BACKSPACE,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_UP,
	KEY_DOWN,
	'1',
	'2',
	'3',
	'4',
	'5',
	'A',
	'D'
};
#endif

}

void UpdateKeyStates()
{
	std::memcpy(prevKeyState, keyState, sizeof(prevKeyState));
#ifdef WINDOWS
	for (size_t i = 0; i < numKeyCodes; ++i)
	{
		keyState[i] = (GetAsyncKeyState(keyCodeToVKey[i]) & 0x8000) ? 1 : 0;
	}
#else
	std::memset(keyState, 0, sizeof(prevKeyState));
	const int ch = getch();
	for (size_t i = 0; i < numKeyCodes; ++i) {
		if (ch == keyCodeToVKey[i]) {
			keyState[i] = 1;
			break;
		}
	}
#endif
}


bool KeyPressed(KeyCode keyCode)
{
	return (keyState[(int)keyCode] == 1);
}

bool KeyJustPressed(KeyCode keyCode)
{
	return (keyState[(int)keyCode] == 1) && (prevKeyState[(int)keyCode] == 0);
}

bool KeyJustReleased(KeyCode keyCode)
{
	return (keyState[(int)keyCode] == 0) && (prevKeyState[(int)keyCode] == 1);
}


bool AnyKeyJustPressed()
{
	bool res = false;
	for (size_t i = 0; i < numKeyCodes; ++i)
	{
		if (KeyJustPressed((KeyCode)i))
		{
			res = true;
			break;
		}
	}
	return res;
}


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


KeyboardInput::KeyboardInput(KeyCode left, KeyCode right) : 
	left { left },
	right { right }
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
