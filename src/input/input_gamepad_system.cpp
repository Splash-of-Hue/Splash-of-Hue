#include "input/input_system.hpp"
#include <iostream>

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

				// If it remains pressed, don't send any new inputs
				if (is_pressed && was_pressed)
				{
					continue;
				}

				// If it has not been pressed, don't send any new inputs
				if (!is_pressed && !was_pressed)
				{
					continue;
				}

				if (is_pressed)
				{
					pressed_controller_buttons[gamepad_id].insert(button_id);
				}
				else
				{
					pressed_controller_buttons[gamepad_id].erase(button_id);
				}
				
				input = {
					INPUT_TYPE::GAMEPAD,
					input_type, (float)i, is_pressed
				};
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

			// If it remains pressed, don't send any new inputs
			if (is_pressed && was_pressed)
			{
				continue;
			}

			// If it has not been pressed, don't send any new inputs
			if (!is_pressed && !was_pressed)
			{
				continue;
			}

			if (is_pressed)
			{
				pressed_controller_buttons[gamepad_id].insert(i);
			}
			else
			{
				pressed_controller_buttons[gamepad_id].erase(i);
			}

			std::cout << "Gamepad button " << i << " input was " << (is_pressed ? "pressed." : "released.") << std::endl;
			Entity gamepad_input_entity;
			Input input = {
				INPUT_TYPE::GAMEPAD,
				SOH_BUTTON_INPUT_TYPE, i, is_pressed
			};
			registry.inputs.insert(gamepad_input_entity, input);
		}
	}
}
