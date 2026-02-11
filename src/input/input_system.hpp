#pragma once

#include "common.hpp"
#include "ecs/registry.hpp"
#include <map>

class InputSystem
{
public:
	GLFWwindow* init(GLFWwindow* window);
	// TODO bDKnCkH1: Remove for later
	void clear_keys();

private:
	// Window handle
	GLFWwindow* window;
	// TODO bDKnCkH1: More robust way of clearing keys
	std::map<int, bool> pressed_keys = {};

	// input callback functions
	void on_window_moved(int xpos, int ypos);
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_scroll(double xoffset, double yoffset);
	void on_mouse_button_pressed(int button, int action, int mods);
};
