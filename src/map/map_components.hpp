#pragma once

#include "common.hpp"
#include "ecs/entity.hpp"
#include "glm/gtx/hash.hpp"
#include "map_constants.hpp"
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct MapTile
{
	int id;
	ivec2 coordinate;
	TILE_TYPE tile_type;
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

struct Teleporter
{
	int level_uid;
	int connected_level_uid;
	/// <summary>
	/// The direction whatever uses teleporter is offset to from the arrival position
	/// </summary>
	DIRECTION offset_direction;
	vec2 arrival_location;
};

// Used for level entity
struct Level
{
	// Unique ID for the room
	int uid;
	// The coordinate of the level in the world grid
	ivec2 coordinate;
	// the 'World Layer" in LDtk, corresponds to which dungeon the Level is on
	int depth = 0; 
	/// Bounding box of walkable floor cells in LDtk pixel space.
	/// Computed after quantized_space_data is built.
	/// Used by MinimapSystem to draw accurate room shapes.
	/// </summary>
	ivec2 floor_ldtk_position = { 0, 0 };
	ivec2 floor_ldtk_size = { 1, 1 };
	// Hallway openings for this room in LDtk pixel space.
	// Each entry is {position, size} of one HallwayReference entity placed in LDtk.
	std::vector<std::pair<ivec2, ivec2>> hallway_ldtk_rects;
	// Neighbor levels, these are the levels connected to this one
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
	bool is_visited = false;
};

struct ChangeMapLevelRequest
{
	Entity moving_entity;
	int new_level_uid;
	vec2 arrival_position;
	vec2 arrival_offset;
	MAP_CHANGE_REQUEST_TYPE type = MAP_CHANGE_REQUEST_TYPE::INVALID;
	DIRECTION offset_direction;
};

struct ColouredWall
{
	std::string id;
};

struct PressurePlateWall
{
	// Is the door open? i.e. is the connected pressure plate currently being pressed
	bool is_open = false;
	std::string id;
};
