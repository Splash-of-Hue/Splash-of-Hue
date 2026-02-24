#pragma once
#include "controls/control_components.hpp"
#include "controls/control_constants.hpp"
#include "input/input_components.hpp"
#include <map>
#include <set>
#include <string>

class ControlInputConversionSystem
{
private:
	std::map<CONTROL_TYPE, float> control_times_ms;
	// TODO edMG9tmb: This is inefficiently checked each time
	std::set<CONTROL_TYPE> processed_types_this_frame;
	Control convert_input(const Input& input);
	Control convert_mouse_button_input(const Input& input);
	Control convert_mouse_scroll_input(const Input& input);
	Control convert_mouse_move_input(const Input& input);
	Control convert_keyboard_input(const Input& input);
	Control convert_joystick_input(const Input& input);
	Control convert_gamepad_input(const Input& input);
	Control generate_control
	(
		std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> control_map,
		int button, int action,
		std::string error_message
	);
	Control generate_control
	(
		std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> control_map,
		std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> modified_control_map,
		int button, int action, int mods,
		std::string error_message
	);

public:
	void step(float elapsed_ms);
};
