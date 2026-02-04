#pragma once

#include "common.hpp"
#include "renderer/render_system.hpp"
#include "ecs/registry.hpp"
#include "world_system.hpp"

#include <map>
#include <bitset>

class InputSystem
{

public:
	InputSystem();

	GLFWwindow* init(GLFWwindow* window, WorldSystem* world_system);

	// TODO m1EAwQf4: This is only being used to make sure that the mouse rectangle isn't being destroyed on world_system reset so maybe don't need this.
	void on_world_reset();

private:
	// Window handle
	GLFWwindow* window;

	// World handle
	WorldSystem* world_system;

	float mouse_pos_x = 0.0f;
	float mouse_pos_y = 0.0f;
	bool shift_key_pressed = false;

	// TODO m1EAwQf4: Remove
	Entity mouse_rect_entity;

	// input callback functions
	void on_window_moved(int xpos, int ypos);
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_scroll(double xoffset, double yoffset);
	void on_mouse_button_pressed(int button, int action, int mods);

	// For movement key handling
	Entity move_keys_entity;
};