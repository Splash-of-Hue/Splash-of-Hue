#pragma once
#include "common.hpp"
#include <vector>

struct MinimapRoom
{
	Entity room_render_entity;
	bool has_render_entity = false;
	std::vector<Entity> hallway_render_entities;
};

struct MinimapPlayerIcon
{

};
