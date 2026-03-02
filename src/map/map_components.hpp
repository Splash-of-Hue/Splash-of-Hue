#pragma once

#include "common.hpp"
#include "glm/gtx/hash.hpp"
#include "map_constants.hpp"
#include <set>
#include <string>
#include <unordered_map>

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


/// <summary>
/// This is the level that the associated entity is in.
/// Only things that can move levels will have this (player, sawfish, etc.)
/// </summary>
struct CurrentLevel
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
	std::set<int> neighbor_level_uids;
	/// <summary>
	// Quantized level data, it is a set of integer coordinates that
	// map to world space coordinates and whether that spot movable.
	// This is used for AI navigation.
	/// </summary>
	std::unordered_map<ivec2, std::pair<vec2, bool>> quantized_space_data;
	/// <summary>
	/// Whether the level is currently loaded.
	/// Since there won't be a level component till a level is loaded,
	/// having this as false means the level has previously been loaded and then unloaded
	/// </summary>
	bool is_loaded = false;
	/// <summary>
	/// The background color of the level, behind everything rendered on screen
	/// </summary>
	vec4 background_color;
};

struct ChangeMapLevelRequest
{
	int new_level_uid;
	vec2 new_player_position;
};

struct ColouredWall
{
	std::string id;
};
