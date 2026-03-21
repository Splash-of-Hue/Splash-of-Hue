#pragma once

// If adding a new Interactable, add that Interactable as a component in interactable_components.hpp 
enum class INTERACTABLE_TYPE
{
	INVALID = 0, 
	BOX = INVALID + 1,
	COLOUR_ORB = BOX + 1,
	MUSHROOM = COLOUR_ORB + 1
};

const int MOSS_FRAME_RATE = 1;

enum class MUSHROOM_SIZE
{
	SMALL = 0,
	MEDIUM = SMALL + 1,
	LARGE = MEDIUM + 1
};

enum class MUSHROOM_WIDTH
{
	SMALL = 25,
	MEDIUM = 100,
	LARGE = 200
};

const MUSHROOM_SIZE DEFAULT_MUSHROOM_SIZE = MUSHROOM_SIZE::MEDIUM;

const vec2 PUZZLE_BUBBLE_SIZE = vec2(80);
const int PUZZLE_BUBBLE_FRAME_RATE = 8;
