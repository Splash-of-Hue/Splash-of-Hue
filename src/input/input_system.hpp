#pragma once

#include "common.hpp"
#include "ecs/registry.hpp"

class InputSystem
{
public:
	GLFWwindow* init(GLFWwindow* window);

private:
	// Window handle
	GLFWwindow* window;

	// input callback functions
	void on_window_moved(int xpos, int ypos);
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_scroll(double xoffset, double yoffset);
	void on_mouse_button_pressed(int button, int action, int mods);
};
