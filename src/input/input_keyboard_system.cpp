#include "input/input_system.hpp"

void InputSystem::step_keyboard(float elapsed_ms)
{
	for (auto& pressed_key : pressed_keys)
	{
		Entity key_event_entity;
		Input input = {
			INPUT_TYPE::KEYBOARD,
			pressed_key.first, pressed_key.second.first, pressed_key.second.second
		};
		registry.inputs.insert(key_event_entity, input);
	}

	for (auto& released_key : released_keys)
	{
		Entity key_event_entity;
		Input input = {
			INPUT_TYPE::KEYBOARD,
			released_key.first, released_key.second.first, released_key.second.second
		};
		registry.inputs.insert(key_event_entity, input);
	}

	released_keys.clear();
}

// on key callback
void InputSystem::on_key(int key, int, int action, int mod)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		pressed_keys[key] = { action, mod };
	}

	if (action == GLFW_RELEASE)
	{
		// Trying to release but the button was not pressed
		if (pressed_keys.count(key) == 0)
		{
			return;
		}

		pressed_keys.erase(key);
		released_keys[key] = { action, mod };
	}
}
