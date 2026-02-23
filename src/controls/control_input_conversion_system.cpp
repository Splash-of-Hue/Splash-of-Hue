#include "controls/control_components.hpp"
#include "controls/control_input_conversion_system.hpp"
#include "ecs/registry.hpp"
#include "input/input_components.hpp"
#include "input/input_constants.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

void ControlInputConversionSystem::step(float elapsed_ms)
{
	processed_types_this_frame.clear();
	registry.controls.clear();

	for (Entity& entity : registry.inputs.entities)
	{
		const Input& input = registry.inputs.get(entity);
		Control& control = convert_input(input);

		if (control.type == CONTROL_TYPE::INVALID)
		{
			continue;
		}

		// TODO edMG9tmb: Inefficient
		if (processed_types_this_frame.count(control.type) == 0)
		{
			control_times_ms[control.type] += elapsed_ms;
			control.time_held_ms = control_times_ms[control.type];
			processed_types_this_frame.insert(control.type);
		}

		registry.controls.emplace(entity, control);
	}

	// Clear all inputs at the end of each update
	registry.inputs.clear();
}

Control ControlInputConversionSystem::convert_input(const Input& input)
{
	switch (input.input_type)
	{
		case INPUT_TYPE::KEYBOARD:
			return convert_keyboard_input(input);
		case INPUT_TYPE::MOUSE_SCROLL:
			return convert_mouse_scroll_input(input);
		case INPUT_TYPE::MOUSE_BUTTON:
			return convert_mouse_button_input(input);
		case INPUT_TYPE::MOUSE_MOVE:
			return convert_mouse_move_input(input);
		case INPUT_TYPE::JOYSTICK:
			return convert_joystick_input(input);
		case INPUT_TYPE::GAMEPAD:
			return convert_gamepad_input(input);
		default:
			assert(false && "Invalid kind of input. Unhandled error.");
	}
}

Control ControlInputConversionSystem::convert_keyboard_input(const Input& input)
{
	int key = input.value_1;
	int action = input.value_2;
	int mods = input.value_3;
	std::string error_message = "Pressed invalid keyboard input: " + std::to_string(key) + ", with modifier: " + std::to_string(mods);

	return generate_control(keyboard_control_map, keyboard_modified_control_map, key, action, mods, error_message);
}

Control ControlInputConversionSystem::convert_mouse_button_input(const Input& input)
{
	Control control = { CONTROL_TYPE::INVALID };
	int button = input.value_1;
	int action = input.value_2;
	int mods = input.value_3;
	std::string error_message = "Pressed invalid mouse input: " + std::to_string(button);

	return generate_control(mouse_control_map, mouse_modified_control_map, button, action, mods, error_message);
}

Control ControlInputConversionSystem::convert_mouse_scroll_input(const Input& input)
{
	Control control = { CONTROL_TYPE::INVALID };
	control.phase = CONTROL_PHASE::END;
	control.value_1 = input.value_1;
	control.value_2 = input.value_2;

	return control;
}

Control ControlInputConversionSystem::convert_mouse_move_input(const Input& input)
{
	Control control = { CONTROL_TYPE::INVALID };
	control.phase = CONTROL_PHASE::END;
	control.value_1 = input.value_1;
	control.value_2 = input.value_2;

	return control;
}

Control ControlInputConversionSystem::convert_joystick_input(const Input& input)
{
	int type = input.value_1;
	bool is_axis_input = type == SOH_AXIS_INPUT_TYPE;
	int button = is_axis_input ? type : input.value_2;
	int action = is_axis_input ? GLFW_PRESS : input.value_3;
	std::string error_message = "Pressed invalid joystick input: " + std::to_string(button);
	Control& control = generate_control(joystick_control_map, button, action, error_message);

	if (is_axis_input)
	{
		bool is_x_axis = (int)input.value_2 % 2 == 0;
		control.value_1 = is_x_axis ? input.value_3 : 0;
		control.value_2 = is_x_axis ? 0 : input.value_3;
	}

	return control;
}

Control ControlInputConversionSystem::convert_gamepad_input(const Input& input)
{
	int type = input.value_1;
	bool is_axis_input = type == SOH_AXIS_INPUT_TYPE;
	int button = input.value_2;
	int action = is_axis_input ? GLFW_PRESS : input.value_3;
	std::string error_message = "Pressed invalid joystick input: " + std::to_string(button);
	Control& control = generate_control(gamepad_control_map, button, action, error_message);

	if (is_axis_input)
	{
		control.value_1 *= input.value_3;
		control.value_2 *= input.value_3;
	}

	return control;
}

Control ControlInputConversionSystem::generate_control
(
	std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> control_map,
	std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> modified_control_map,
	int button, int action, int mods,
	std::string error_message
)
{
	Control control = { CONTROL_TYPE::INVALID };
	std::pair<CONTROL_TYPE, std::vector<float>> control_map_values;

	try
	{
		// If there are no modifiers, use the normal map
		if (mods == 0)
		{
			control_map_values = control_map.at(button);
		}
		else
		{
			control_map_values = modified_control_map.at(button).at(mods);
		}
	}
	catch (const std::out_of_range&)
	{
		std::cout << error_message << std::endl;
		return control;
	}

	control.type = control_map_values.first;
	control.phase = action == GLFW_PRESS ? CONTROL_PHASE::START
		: action == GLFW_RELEASE ? CONTROL_PHASE::END : CONTROL_PHASE::SUSTAIN;
	std::vector<float>& control_values = control_map_values.second;

	if (control_values.size() > 0)
	{
		control.value_1 = control_values[0];
	}

	if (control_values.size() > 1)
	{
		control.value_2 = control_values[1];
	}

	return control;
}

Control ControlInputConversionSystem::generate_control
(
	std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> control_map,
	int button, int action,
	std::string error_message
)
{
	return generate_control(control_map, {}, button, action, 0, error_message);
}
