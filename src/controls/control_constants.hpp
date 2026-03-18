#pragma once
#include "common.hpp" // For GLFW
#include <map>
#include <utility>
#include <vector>
#include <input/input_constants.hpp>

enum class CONTROL_TYPE
{
	INVALID,
	MOVE,
	CHANGE_COLOR,
	RESTART,
	DEBUG,
	END_GAME,
	TOGGLE_COLOUR_MENU,
	CHANGE_COLOUR_MENU_SELECTION,
	SET_COLOUR,
	PAUSE,
	CONFIRM,
	CANCEL
};

enum class CONTROL_PHASE
{
	START,
	SUSTAIN,
	END
};

enum class COLOUR_CONTROL_VALUE : int
{
	NEUTRAL = 0,
	BLUE = 1,
	RED = 2,
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> joystick_axis_control_map = {
	{ SOH_JOYSTICK_AXIS_LEFT_X, { CONTROL_TYPE::MOVE, { } } },
	{ SOH_JOYSTICK_AXIS_LEFT_Y, { CONTROL_TYPE::MOVE, { } } },
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> joystick_button_control_map = {
	{ SOH_JOYSTICK_BUTTON_L_SHOULDER, { CONTROL_TYPE::SET_COLOUR , { static_cast<float>(COLOUR_CONTROL_VALUE::BLUE) } } },
	{ SOH_JOYSTICK_BUTTON_R_SHOULDER, { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::RED) } } },
	{ SOH_JOYSTICK_BUTTON_Y, { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::NEUTRAL) } } },
	{ SOH_JOYSTICK_BUTTON_A, { CONTROL_TYPE::CONFIRM, { } } },
	{ SOH_JOYSTICK_BUTTON_B, { CONTROL_TYPE::CANCEL, { } } },
	{ SOH_JOYSTICK_BUTTON_START, { CONTROL_TYPE::PAUSE, { } } },
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> gamepad_axis_control_map = {
	{ GLFW_GAMEPAD_AXIS_LEFT_X, {CONTROL_TYPE::MOVE, { 1, 0 } } },
	{ GLFW_GAMEPAD_AXIS_LEFT_Y, {CONTROL_TYPE::MOVE, { 0, 1 } } },
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> gamepad_button_control_map = {
	{ GLFW_GAMEPAD_BUTTON_X, { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::BLUE) } } },
	{ GLFW_GAMEPAD_BUTTON_B, { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::RED) } } },
	{ GLFW_GAMEPAD_BUTTON_Y, { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::NEUTRAL) } } },
	{ GLFW_GAMEPAD_BUTTON_A, { CONTROL_TYPE::CONFIRM, { } } },
	{ GLFW_GAMEPAD_BUTTON_START, { CONTROL_TYPE::PAUSE, { } } },
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> keyboard_control_map = {
	{ GLFW_KEY_W, { CONTROL_TYPE::MOVE, {  0.0f, -1.0f } } },
	{ GLFW_KEY_A, { CONTROL_TYPE::MOVE, { -1.0f,  0.0f } } },
	{ GLFW_KEY_S, { CONTROL_TYPE::MOVE, {  0.0f,  1.0f } } },
	{ GLFW_KEY_D, { CONTROL_TYPE::MOVE, {  1.0f,  0.0f } } },
	{ GLFW_KEY_R, { CONTROL_TYPE::RESTART, { } } }, // TODO: Change to system state
	{ GLFW_KEY_TAB, { CONTROL_TYPE::DEBUG, { } } }, // TODO: Revisit for actual debug mode
	{ GLFW_KEY_ESCAPE, { CONTROL_TYPE::PAUSE, { } } },
	{ GLFW_KEY_SPACE, { CONTROL_TYPE::CONFIRM, { } } },
	{ GLFW_KEY_ENTER, { CONTROL_TYPE::CONFIRM, { } } },
	{ GLFW_KEY_LEFT,  { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::BLUE) } } },
	{ GLFW_KEY_DOWN,  { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::NEUTRAL) } } },
	{ GLFW_KEY_RIGHT, { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::RED) } } },
	{ GLFW_KEY_UP,    { CONTROL_TYPE::SET_COLOUR, { static_cast<float>(COLOUR_CONTROL_VALUE::NEUTRAL) } } },
};

// This is for any mouse inputs with modifiers (i.e. Shift, Alt, Control)
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> keyboard_modified_control_map = {
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> mouse_control_map = {
};

// This is for any mouse inputs with modifiers (i.e. Shift, Alt, Control)
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> mouse_modified_control_map = {
};
