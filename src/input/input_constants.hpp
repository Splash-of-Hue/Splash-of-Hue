#pragma once

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
	// value_1 = SOH_JOYSTICK_AXIS, value_2 = axis 1, value_3 = axis 2
	// value_1 = SOH_JOYSTICK_BUTTON, value_2 = button, value_3 = action
	JOYSTICK,
	GAMEPAD,
};

#define SOH_AXIS_INPUT_TYPE 0
#define SOH_BUTTON_INPUT_TYPE 1

// TODO: Remove temporary
#define SOH_JOYSTICK_AXIS_LEFT_X 0
#define SOH_JOYSTICK_AXIS_LEFT_Y 1
#define SOH_JOYSTICK_AXIS_RIGHT_X 2
#define SOH_JOYSTICK_AXIS_RIGHT_Y 3
#define SOH_JOYSTICK_BUTTON_Y 2
#define SOH_JOYSTICK_BUTTON_L_SHOULDER 12
#define SOH_JOYSTICK_BUTTON_R_SHOULDER 4

// How many milliseconds pass before requesting the next state check on joysticks
const float INPUT_POLLING_RATE_MS = 10.0f;
// The deadzone for joystick and gamepad sticks
const float INPUT_DEADZONE_SIZE = 0.1f;
