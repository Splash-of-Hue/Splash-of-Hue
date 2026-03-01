#pragma once

#include "common.hpp"
#include "map_constants.hpp"
#include <unordered_map>
#include "glm/gtx/hash.hpp"

struct MapTile
{
	int id;
	ivec2 coordinate;
	MAP_TILE_TYPE tile_type;
};

// Associates a maptile to the level it is in
struct LevelTile
{
	int level_uid;
};

// Associates an entity to the level it is in
struct LevelEntity
{
	int level_uid;
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
	// Neighbor levels, these are the levels connected to this one
	std::vector<int> neighbor_level_uids;
	// Quantized level data, it is a set of integer coordinates that
	// map to world space coordinates and whether that spot movable.
	// This is used for AI navigation.
	std::unordered_map<ivec2, std::pair<vec2, bool>> quantized_space_data;
};

struct ChangeMapLevelRequest
{
	int new_level_uid;
	vec2 new_player_position;
};
