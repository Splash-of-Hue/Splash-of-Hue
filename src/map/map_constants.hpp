#pragma once

enum class MAP_CHANGE_REQUEST_TYPE
{
	INVALID,
	PLAYER,
	CREATURE
};

enum TILE_TYPE
{
	DEFAULT = 0,
	WALL = 1 << 0,
};

inline TILE_TYPE operator|(TILE_TYPE a, TILE_TYPE b)
{
	return static_cast<TILE_TYPE>(static_cast<int>(a) | static_cast<int>(b));
}

inline TILE_TYPE operator&(TILE_TYPE a, TILE_TYPE b)
{
	return static_cast<TILE_TYPE>(static_cast<int>(a) & static_cast<int>(b));
}
