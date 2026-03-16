#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"

// Getting Sizes of Emissions
std::map<int, float> typeOfEntityToEmissionSize = {
	{0, 150}, //player
	{1, 100}, //small mushroom
	{2, 300}, //medium mushroom
	{3, 500}, //large mushroom
};
