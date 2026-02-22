#include "input/input_system.hpp"
#include <iostream>

static bool make_button_input_component(
	bool is_pressed,
	bool was_pressed,
	int button_id,
	int gamepad_id,
	std::map<int, std::set<int>>& pressed_buttons,
	Input& input
)
{
	// If it remains pressed, don't send any new inputs
	if (is_pressed && was_pressed)
	{
		return false;
	}

	// If it has not been pressed, don't send any new inputs
	if (!is_pressed && !was_pressed)
	{
		return false;
	}

	if (is_pressed)
	{
		pressed_buttons[gamepad_id].insert(button_id);
	}
	else
	{
		pressed_buttons[gamepad_id].erase(button_id);
	}

	std::cout << "Gamepad button " << button_id << " input was " << (is_pressed ? "pressed." : "released.") << std::endl;
	input = {
		INPUT_TYPE::GAMEPAD,
		(float)SOH_BUTTON_INPUT_TYPE, (float)button_id, (float)is_pressed
	};

	return true;
}

void InputSystem::step_gamepads(float elapsed_ms)
{
	for (auto gamepad_id : connected_gamepads)
	{
		GLFWgamepadstate gamepad_state;
		const auto state_acquired = glfwGetGamepadState(gamepad_id, &gamepad_state);

		if (!state_acquired)
		{
			continue;
		}

		// Look at each axis, including the triggers
		for (auto i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; i++)
		{
			auto axis = gamepad_state.axes[i];
			bool is_trigger = false;

			if (i == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER || i == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER)
			{
				is_trigger = true;
			}

			// If it's a stick and not a trigger, check if it passes the deadzone
			if (!is_trigger && abs(axis) < INPUT_DEADZONE_SIZE)
			{
				continue;
			}

			float input_type = SOH_AXIS_INPUT_TYPE;
			Entity gamepad_input_entity;
			Input input;

			if (is_trigger)
			{
				const float INPUT_TRIGGER_DEADZONE = 0.02f;
				input_type = SOH_BUTTON_INPUT_TYPE;
				// Add the total number of buttons so there isn't an overlap with the actual buttons
				auto button_id = GLFW_GAMEPAD_BUTTON_LAST + i;
				float is_pressed = axis > (INPUT_TRIGGER_DEADZONE - 1);
				auto was_pressed = pressed_controller_buttons[gamepad_id].count(button_id);

				if (!make_button_input_component(is_pressed, was_pressed, button_id, gamepad_id, pressed_controller_buttons, input))
				{
					continue;
				}
			}
			else
			{
				input = {
					INPUT_TYPE::GAMEPAD,
					input_type, (float)i, axis
				};
			}

			std::cout << "Gamepad axis " << i << " has value: " << axis << std::endl;
			registry.inputs.insert(gamepad_input_entity, input);
		}

		for (auto i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++)
		{
			auto is_pressed = gamepad_state.buttons[i];
			auto was_pressed = pressed_controller_buttons[gamepad_id].count(i) > 0;
			Input input;

			if (!make_button_input_component(is_pressed, was_pressed, i, gamepad_id, pressed_controller_buttons, input))
			{
				continue;
			}

			Entity gamepad_input_entity;
			registry.inputs.insert(gamepad_input_entity, input);
		}
	}
}
