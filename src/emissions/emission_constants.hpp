#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include <map>

const enum EMISSION_SIZE
{
	PLAYER_EMISSION = 200,
	MUSHROOM_SMALL_EMISSION = 100,
	MUSHROOM_MEDIUM_EMISSION = 300,
	MUSHROOM_LARGE_EMISSION = 500,
	ANGLERFISH_EMISSION = 100
};

const std::map<std::string, vec2> EMISSION_OFFSET =
{
	{"PLAYER", {0,-1}},
	{"MUSHROOM", {0,-1}},
	{"ANGLERFISH", {50, -17}} // TODO 3KZ4EsT4
	//Angerfish offset will be dynamic with its rotation once it's implemented
};

const float TIME_TO_GROW = 1000.f;
