#include "input/input_components.hpp"
#include "input/input_system.hpp"
#include "input_constants.hpp"
#include "ecs/entity.hpp"
#include <iostream>

inline float usb_gamecube_correction_x(const float& value) { return (value - 0.036255) * 1.75044593; };
inline float usb_gamecube_correction_y(const float& value) { return (value + 0.016266) * 1.61603364; };

void InputSystem::step_joysticks(float elapsed_ms)
{
	for (auto joystick_id : connected_joysticks)
	{
		int axis_count;
		const float* axes = glfwGetJoystickAxes(joystick_id, &axis_count);

		for (auto i = 0; i < axis_count; i++)
		{
			auto axis = axes[i];
			// Correction for inverted Y axes, and gamecube specifically
			axis = (i % 2 == 0) ? usb_gamecube_correction_x(axis) : usb_gamecube_correction_y(-axis);

			// If it's a stick and not a trigger, check if it passes the deadzone
			if (abs(axis) < INPUT_DEADZONE_SIZE)
			{
				continue;
			}

			std::cout << "Joystick axis " << i << " has value: " << axis << std::endl;
			Entity gamepad_input_entity;
			Input input = {
				INPUT_TYPE::JOYSTICK,
				SOH_AXIS_INPUT_TYPE, (float)i, axis
			};
			registry.inputs.insert(gamepad_input_entity, input);
		}

		int button_count;
		const unsigned char* buttons = glfwGetJoystickButtons(joystick_id, &button_count);

		for (auto i = 0; i < button_count; i++)
		{
			auto is_pressed = buttons[i];
			auto was_pressed = pressed_controller_buttons[joystick_id].count(i) > 0;

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
				pressed_controller_buttons[joystick_id].insert(i);
			}
			else
			{
				pressed_controller_buttons[joystick_id].erase(i);
			}

			std::cout << "Joystick button " << i << " input was " << (is_pressed ? "pressed." : "released.") << std::endl;
			Entity joystick_entity;
			Input input = {
				INPUT_TYPE::JOYSTICK,
				SOH_BUTTON_INPUT_TYPE, i, is_pressed
			};
			registry.inputs.insert(joystick_entity, input);
		}
	}
}
