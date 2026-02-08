#pragma once
#include "common.hpp" // For GLFW
#include <map>
#include <utility>
#include <vector>

enum class CONTROL_TYPE
{
	INVALID,
	MOVE,
	CHANGE_COLOR,
	// TODO Ze15bn1r: Move to game state
	RESTART,
	DEBUG,
	END_GAME,
	TOGGLE_COLOUR_MENU,
	CHANGE_COLOUR_MENU_SELECTION,
};

enum class CONTROL_PHASE
{
	START,
	SUSTAIN,
	END
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> keyboard_control_map = {
	{ GLFW_KEY_W, { CONTROL_TYPE::MOVE, {  0.0f, -1.0f } } },
	{ GLFW_KEY_A, { CONTROL_TYPE::MOVE, { -1.0f,  0.0f } } },
	{ GLFW_KEY_S, { CONTROL_TYPE::MOVE, {  0.0f,  1.0f } } },
	{ GLFW_KEY_D, { CONTROL_TYPE::MOVE, {  1.0f,  0.0f } } },
	{ GLFW_KEY_R, { CONTROL_TYPE::RESTART, { } } }, // TODO: Change to system state
	{ GLFW_KEY_TAB, { CONTROL_TYPE::DEBUG, { } } }, // TODO: Revisit for actual debug mode
	{ GLFW_KEY_ESCAPE, { CONTROL_TYPE::END_GAME, { } } }, // TODO: Change to system state
	{ GLFW_KEY_E, {CONTROL_TYPE::TOGGLE_COLOUR_MENU, { } } },
	{ GLFW_KEY_LEFT, { CONTROL_TYPE::CHANGE_COLOUR_MENU_SELECTION, { -1 } } },
	{ GLFW_KEY_RIGHT, { CONTROL_TYPE::CHANGE_COLOUR_MENU_SELECTION, { 1 } } },
	{ GLFW_KEY_UP, { CONTROL_TYPE::CHANGE_COLOUR_MENU_SELECTION, { -1 } } },
	{ GLFW_KEY_DOWN, { CONTROL_TYPE::CHANGE_COLOUR_MENU_SELECTION, { 1 } } },
};

// This is for any mouse inputs with modifiers (i.e. Shift, Alt, Control)
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> keyboard_modified_control_map = {
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> mouse_control_map = {
};

// This is for any mouse inputs with modifiers (i.e. Shift, Alt, Control)
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> mouse_modified_control_map = {
};
