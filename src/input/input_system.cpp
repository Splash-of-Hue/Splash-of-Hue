#include "input/input_system.hpp"
#include "input/input_components.hpp"
#include <iostream>

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

	glfwSetWindowPosCallback(window, window_moved_redirect);
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_pressed_redirect);
	glfwSetScrollCallback(window, mouse_scroll_redirect);

	return window;
}

// on key callback
void InputSystem::on_key(int key, int, int action, int mod)
{
	Entity key_event_entity;
	Input input = {
		INPUT_TYPE::KEYBOARD,
		key, action, mod
	};
	registry.inputs.insert(key_event_entity, input);
}

void InputSystem::on_mouse_move(vec2 mouse_position)
{
	// Make mouse scroll entity and add the input
	Entity mouse_move_entity;
	Input input =  {
		INPUT_TYPE::MOUSE_MOVE,
		mouse_position.x, mouse_position.y
	};
	registry.inputs.insert(mouse_move_entity, input);
}

// TODO LtiFv3Nh: Move to a window system
void InputSystem::on_window_moved(int xpos, int ypos)
{
	std::cout << "Window position, X: " << xpos << ", Y: " << ypos << std::endl;
}

void InputSystem::on_mouse_scroll(double xoffset, double yoffset)
{
	// Make mouse scroll entity and add the input
	Entity mouse_button_entity;
	Input input = {
		INPUT_TYPE::MOUSE_SCROLL,
		xoffset, yoffset
	};
	registry.inputs.insert(mouse_button_entity, input);
}

void InputSystem::on_mouse_button_pressed(int button, int action, int mods)
{
	// Make mouse button entity and add the input
	Entity mouse_button_entity;
	Input input = {
		INPUT_TYPE::MOUSE_BUTTON,
		button, action, mods
	};
	registry.inputs.insert(mouse_button_entity, input);
}
