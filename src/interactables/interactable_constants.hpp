#pragma once

// If adding a new Interactable, add that Interactable as a component in interactable_components.hpp 
enum class INTERACTABLE_TYPE
{
	INVALID = 0, 
	BOX = INVALID + 1,
	COLOUR_ORB = BOX + 1,
	MUSHROOM = COLOUR_ORB + 1
};
