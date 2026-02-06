#pragma once
#include "input/input_constants.hpp"

struct Input
{
	INPUT_TYPE input_type;
	// What each value is will depend on the input type
	int value_1 = 0;
	int value_2 = 0;
	int value_3 = 0;
};
