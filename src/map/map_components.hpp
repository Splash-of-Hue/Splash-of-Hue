#pragma once

#include "common.hpp"

struct MapTile
{
	int id;
	ivec2 coordinate;
};

struct SpawnPoint
{
	int level_uid;
	int connected_level_uid;
	vec2 send_to_location;
};

// Used for level entity
struct Level
{
	// Unique ID for the room
	int uid;
	// The coordinate of the level in the world grid
	ivec2 coordinate;
};

struct ChangeMapLevelRequest
{
	int new_level_uid;
	vec2 new_player_position;
};
