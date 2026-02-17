#pragma once

#include "common.hpp"
#include "ecs/registry.hpp"
#include "input/input_constants.hpp"
#include <map>
#include <set>
#include <string>

class InputSystem
{
private:
	// Window handle
	GLFWwindow* window;
	std::map<int, std::pair<int, int>> pressed_keys = {};
	std::map<int, std::pair<int, int>> released_keys = {};
	// Not all joysticks are gamepads, but gamepads provide a more clear set of input identifiers.
	// Instead of checking every step if a joystick is a gamepad, store them independently
	std::set<int> connected_joysticks = {};
	std::set<int> connected_gamepads = {};
	std::map<int, std::set<int>> pressed_controller_buttons = {};
	// Store the names because when they are disconnected, they can't get requested
	std::map<unsigned int, std::string> controller_names = {};
	// The time since last poll of input (for joysticks and gamepads)
	float polling_time = 0;

	// different systems for input type
	void step_keyboard(float elapsed_ms);
	void step_joysticks(float elapsed_ms);
	void step_gamepads(float elapsed_ms);

	// input callback functions
	void on_window_moved(int xpos, int ypos);
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_scroll(double xoffset, double yoffset);
	void on_mouse_button_pressed(int button, int action, int mods);
	void on_joystick_connection_changed(int jid, int event);
	// helper functions
	const std::string get_connected_controller_name(int jid, bool is_gamepad);


public:
	GLFWwindow* InputSystem::init(GLFWwindow* window)
	{
		// Setting callbacks to member functions (that's why the redirect is needed)
		// Input is handled using GLFW, for more info see
		// http://www.glfw.org/docs/latest/input_guide.html

		this->window = window;

		glfwSetWindowUserPointer(window, this);
		auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
		auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
		auto mouse_button_pressed_redirect = [](GLFWwindow* wnd, int _button, int _action, int _mods) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button_pressed(_button, _action, _mods); };
		auto mouse_scroll_redirect = [](GLFWwindow* wnd, double _xoffset, double _yoffset) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_scroll(_xoffset, _yoffset); };
		auto window_moved_redirect = [](GLFWwindow* wnd, int _xpos, int _ypos) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_window_moved(_xpos, _ypos); };
		auto joystick_connection_change_redirect = [](int jid, int event) { ((InputSystem*)glfwGetJoystickUserPointer(jid))->on_joystick_connection_changed(jid, event); };

		glfwSetWindowPosCallback(window, window_moved_redirect);
		glfwSetKeyCallback(window, key_redirect);
		glfwSetCursorPosCallback(window, cursor_pos_redirect);
		glfwSetMouseButtonCallback(window, mouse_button_pressed_redirect);
		glfwSetScrollCallback(window, mouse_scroll_redirect);
		glfwSetJoystickCallback(joystick_connection_change_redirect);

		for (auto jid = 0; jid <= GLFW_JOYSTICK_LAST; jid++)
		{
			// If the controller isn't there, continue on
			if (!glfwJoystickPresent(jid))
			{
				continue;
			}

			glfwSetJoystickUserPointer(jid, this);
			on_joystick_connection_changed(jid, GLFW_CONNECTED);
		}

		return window;
	}

	void InputSystem::step(float elapsed_ms)
	{
		if (registry.inputClearRequests.size() > 0)
		{
			pressed_keys.clear();
			registry.inputClearRequests.clear();
		}

		polling_time += elapsed_ms;

		if (polling_time < INPUT_POLLING_RATE_MS)
		{
			return;
		}

		polling_time = 0;

		step_keyboard(elapsed_ms);
		step_joysticks(elapsed_ms);
		step_gamepads(elapsed_ms);
	}
};
