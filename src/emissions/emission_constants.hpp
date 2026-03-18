#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include <map>

const enum EMISSION_SIZE
{
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
	{"ANGLERFISH", {20, 0}} //we will calculate this number soon https://trello.com/c/0dTrDDAQ/109-2-creature-behaviour-system-angler-fish
};

const float TIME_TO_GROW = 1000.f;
