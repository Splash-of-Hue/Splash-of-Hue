#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include <map>

// Basically an Enum for getting the size of an emissions radius. Because mushrooms are already an enum of values 0-2, having a map of ints
// means we can use the mushrooms enum.
const enum EMISSION_SIZE
{
	//everything has _EMISSION in its name because ANGLERFISH is already being used in the creature_constants enum, and c++ is telling me its a redefinition
	//so to be consistent, everything just gets _EMISSION now
	PLAYER_EMISSION = 150,
	MUSHROOM_SMALL_EMISSION = 100,
	MUSHROOM_MEDIUM_EMISSION = 300,
	MUSHROOM_LARGE_EMISSION = 500,
	ANGLERFISH_EMISSION = 350
};

const std::map<std::string, vec2> EMISSION_OFFSET =
{
	{"PLAYER", {0,0}},
	{"MUSHROOM", {0,0}},
	{"ANGLERFISH", {20, 0}} //we will calculate this number soon

};

const float TIME_TO_GROW = 1000.f;
