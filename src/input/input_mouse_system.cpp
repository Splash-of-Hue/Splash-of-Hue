#include "input/input_system.hpp"

void InputSystem::on_mouse_move(vec2 mouse_position)
{
	// Make mouse scroll entity and add the input
	Entity mouse_move_entity;
	Input input = {
		INPUT_TYPE::MOUSE_MOVE,
		mouse_position.x, mouse_position.y
	};
	registry.inputs.insert(mouse_move_entity, input);
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
