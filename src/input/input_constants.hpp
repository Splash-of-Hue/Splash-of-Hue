#pragma once

enum MoveKey
{
	W = 0,
	A = 1,
	S = 2,
	D = 3
};

enum class INPUT_TYPE
{
	// value_1 = xoffset, value_2 = yoffset, value_3 = N/A
	MOUSE_SCROLL,
	// value_1 = button, value_2 = action, value_3 = mods
	MOUSE_BUTTON,
	// value_1 = x_position, value_2 = y_position, value_3 = N/A
	MOUSE_MOVE,
	// value_1 = key, value_2 = action, value_3 = mods
	KEYBOARD,
	// TODO
	GAMEPAD,
};
