#pragma once
#include "input/input_constants.hpp"

struct Input
{
	INPUT_TYPE input_type;
	// What each value is will depend on the input type
	float value_1 = 0;
	float value_2 = 0;
	float value_3 = 0;
};

struct InputClearRequest
{

};
