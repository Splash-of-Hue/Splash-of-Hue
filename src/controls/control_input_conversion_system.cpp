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
	for (Entity& entity : registry.inputs.entities)
	{
		Input input = registry.inputs.get(entity);
		Control control = convert_input(input);

		if (control.type == CONTROL_TYPE::INVALID)
		{
			continue;
		}

		// TODO edMG9tmb: Issue where if multiple things have the same control, it increases time more
		control_times_ms[control.type] += elapsed_ms;
		control.time_held_ms = control_times_ms[control.type];

		registry.controls.emplace(entity, control);
	}

	// Clear all inputs at the end of each update
	registry.inputs.clear();
}

Control ControlInputConversionSystem::convert_input(Input input)
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
		default:
			assert(false && "Invalid kind of input. Unhandled error.");
	}
}

Control ControlInputConversionSystem::convert_keyboard_input(Input input)
{
	int key = input.value_1;
	int action = input.value_2;
	int mods = input.value_3;
	std::pair<CONTROL_TYPE, std::vector<float>> control_map_values;
	Control control = { CONTROL_TYPE::INVALID };

	try
	{
		// If there are no modifiers, use the normal map
		if (mods == 0)
		{
			control_map_values = keyboard_control_map.at(key);
		}
		else
		{
			control_map_values = keyboard_modified_control_map.at(key).at(mods);
		}
	}
	catch (const std::out_of_range&)
	{
		std::cout << "Pressed invalid keyboard input: " << key << ", with modifier: " << mods << std::endl;
		return control;
	}

	control.type = control_map_values.first;
	control.phase = action == GLFW_PRESS ? CONTROL_PHASE::START
		: action == GLFW_RELEASE ? CONTROL_PHASE::END : CONTROL_PHASE::SUSTAIN;
	auto& control_values = control_map_values.second;

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

Control ControlInputConversionSystem::convert_mouse_button_input(Input input)
{
	Control control = { CONTROL_TYPE::INVALID };
	std::pair<CONTROL_TYPE, std::vector<float>> control_map_values;
	int button = input.value_1;
	int action = input.value_2;
	int mods = input.value_3;
		
	try
	{
		// If there are no modifiers, use the normal map
		if (mods == 0)
		{
			control_map_values = mouse_control_map.at(button);
		}
		else
		{
			control_map_values = mouse_modified_control_map.at(button).at(mods);
		}
	}
	catch (const std::out_of_range&)
	{
		std::cout << "Pressed invalid mouse input: " << button << std::endl;
		return control;
	}

	control.type = control_map_values.first;
	control.phase = action == GLFW_PRESS ? CONTROL_PHASE::START
		: action == GLFW_RELEASE ? CONTROL_PHASE::END : CONTROL_PHASE::SUSTAIN;
	auto& control_values = control_map_values.second;

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

Control ControlInputConversionSystem::convert_mouse_scroll_input(Input input)
{
	Control control = { CONTROL_TYPE::CHANGE_RECT_TEST_SIZE };
	control.phase = CONTROL_PHASE::END;
	control.value_1 = input.value_1;
	control.value_2 = input.value_2;

	return control;
}

Control ControlInputConversionSystem::convert_mouse_move_input(Input input)
{
	Control control = { 
		CONTROL_TYPE::MOVE_RECT_TEST,
		CONTROL_PHASE::END,
		input.value_1,
		input.value_2
	};

	return control;
}
