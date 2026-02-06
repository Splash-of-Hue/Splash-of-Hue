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
	// TODO m1EAwQf4: Remove this when no longer need to manually make rectangles for test
	// Map numpad to increasing or decreasing the RGB value of rect on screen
	CHANGE_RECT_TEST_COLOR,
	CHANGE_RECT_TEST_SIZE,
	MOVE_RECT_TEST,
	PLACE_RECT_TEST,
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
	// TODO m1EAwQf4: Remove this when no longer need to manually make rectangles for test
	{ GLFW_KEY_KP_1, { CONTROL_TYPE::CHANGE_RECT_TEST_COLOR, { 0, -0.01 } } },
	{ GLFW_KEY_KP_2, { CONTROL_TYPE::CHANGE_RECT_TEST_COLOR, { 1, -0.01 } } },
	{ GLFW_KEY_KP_3, { CONTROL_TYPE::CHANGE_RECT_TEST_COLOR, { 2, -0.01 } } },
	{ GLFW_KEY_KP_4, { CONTROL_TYPE::CHANGE_RECT_TEST_COLOR, { 0,  0.01 } } },
	{ GLFW_KEY_KP_5, { CONTROL_TYPE::CHANGE_RECT_TEST_COLOR, { 1,  0.01 } } },
	{ GLFW_KEY_KP_6, { CONTROL_TYPE::CHANGE_RECT_TEST_COLOR, { 2,  0.01 } } },
};

// This is for any mouse inputs with modifiers (i.e. Shift, Alt, Control)
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> keyboard_modified_control_map = {
};


const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> mouse_control_map = {
	// TODO m1EAwQf4: Remove this when no longer need to manually make rectangles for test
	{ GLFW_MOUSE_BUTTON_LEFT, { CONTROL_TYPE::PLACE_RECT_TEST, { }}},
};

// This is for any mouse inputs with modifiers (i.e. Shift, Alt, Control)
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> mouse_modified_control_map = {
};
