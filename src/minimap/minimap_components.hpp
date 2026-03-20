#pragma once
#include "common.hpp"
#include <vector>

struct MinimapRoom
{
	ivec2 floor_ldtk_position = { 0, 0 };
	ivec2 floor_ldtk_size = { 1, 1 };
	Entity room_render_entity;
	bool has_render_entity = false;
	std::vector<Entity> hallway_render_entities;
};

struct MinimapPlayerIcon
{

};
