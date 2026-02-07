#pragma once
#include "controls/control_constants.hpp"

struct Control
{
	CONTROL_TYPE type;
	CONTROL_PHASE phase;
	// Different purposes depending on the control type.
	// TODO SnB9A38K: Could refactor for player controls
	float value_1 = 0;
	float value_2 = 0;
	float time_held_ms = 0;
};
