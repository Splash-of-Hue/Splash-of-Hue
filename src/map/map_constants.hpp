#pragma once

enum class MAP_CHANGE_REQUEST_TYPE
{
	INVALID,
	PLAYER,
	CREATURE
};

const float TELEPORTER_RADIUS = 10.0f;

enum MAP_TILE_TYPE
{
	DEFAULT = 0,
	WALL = 1 << 0,
};

inline MAP_TILE_TYPE operator|(MAP_TILE_TYPE a, MAP_TILE_TYPE b)
{
	return static_cast<MAP_TILE_TYPE>(static_cast<int>(a) | static_cast<int>(b));
}

inline MAP_TILE_TYPE operator&(MAP_TILE_TYPE a, MAP_TILE_TYPE b)
{
	return static_cast<MAP_TILE_TYPE>(static_cast<int>(a) & static_cast<int>(b));
}
