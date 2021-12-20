#include "../Keyboard.h"
#include <cstring>
#include <windows.h>

namespace
{


	int prevKeyState[numKeyCodes] = { -1 };
	int keyState[numKeyCodes] = { -1 };

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
}

bool InitKeyboard() {
	return true;
}

void UpdateKeyStates()
{
	std::memcpy(prevKeyState, keyState, sizeof(prevKeyState));
	for (size_t i = 0; i < numKeyCodes; ++i)
	{
		keyState[i] = (GetAsyncKeyState(keyCodeToVKey[i]) & 0x8000) ? 1 : 0;
	}
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
