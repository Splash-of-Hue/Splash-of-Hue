#pragma once
#include "controls/control_components.hpp"
#include "controls/control_constants.hpp"
#include "input/input_components.hpp"
#include <map>

class ControlInputConversionSystem
{
private:
	std::map<CONTROL_TYPE, float> control_times_ms;
	Control convert_input(Input input);
	Control convert_mouse_button_input(Input input);
	Control convert_mouse_scroll_input(Input input);
	Control convert_mouse_move_input(Input input);
	Control convert_keyboard_input(Input input);

public:
	void step(float elapsed_ms);
};
