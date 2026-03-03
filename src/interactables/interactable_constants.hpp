#pragma once

// If adding a new Interactable, add that Interactable as a component in interactable_components.hpp 
enum class INTERACTABLE_TYPE
{
	INVALID = 0, 
	BOX = INVALID + 1,
	COLOUR_ORB = BOX + 1,
	MUSHROOM = COLOUR_ORB + 1,
	CONTROLS = MUSHROOM + 1
};

enum class CONTROL_PNG_TYPE
{
	INVALID = 0,
	WASD = INVALID + 1,
	ARROW_RED = WASD + 1,
	ARROW_NO_RED = ARROW_RED + 1,
	COLOUR_AFFECTS = ARROW_NO_RED + 1
};

const int MOSS_FRAME_RATE = 1;
