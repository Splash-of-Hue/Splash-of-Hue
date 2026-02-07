#pragma once
#include "controls/control_components.hpp"
#include "controls/control_constants.hpp"
#include "input/input_components.hpp"
#include <map>

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

public:
	void step(float elapsed_ms);
};
