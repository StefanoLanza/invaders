#pragma once

enum class KeyCode
{
	escape,
	enter,
	spaceBar,
	left,
	right,
	up,
	down,
	_1,
	_2,
	_3,
	_4,
	_5,
	A,
	D
};
constexpr int numKeyCodes = static_cast<int>(KeyCode::D) + 1;

bool InitKeyboard();
void UpdateKeyStates();
bool KeyPressed(KeyCode keyCode);
bool KeyJustPressed(KeyCode keyCode);
bool KeyJustReleased(KeyCode keyCode);
bool AnyKeyJustPressed();
