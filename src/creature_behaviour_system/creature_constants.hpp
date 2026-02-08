#pragma once

#include <map>

enum CREATURETYPE
{
	ANGLERFISH = 0,
	CRAB = ANGLERFISH + 1,
	MINNOWS = CRAB + 1,
	SAWFISH = MINNOWS + 1,
	SHRIMP = SAWFISH + 1,
	WHALE = SHRIMP + 1,
	UNDEFINED = WHALE + 1
};

struct Creature
{
	CREATURETYPE creatureType = UNDEFINED;
	// colour type here from Abby's colour system
};

// Creature specific constants
struct Anglerfish
{ // could have a pointer to its ball of light?
};

struct Crab
{
};

struct Minnows
{ // could have a pointer to leader minnow
};

struct Sawfish
{
};

enum DIRECTION
{
	UP = 0,
	DOWN = UP + 1,
	LEFT = DOWN + 1,
	RIGHT = LEFT + 1
};

struct Shrimp
{
	// Tracks current direction shrimp is facing.
	DIRECTION currDirect = UP;
};

struct Whale
{
};


// for creatures that pathfind. TODO, edit based on experiments with how close each creature should be
constexpr float pathfinderMinDistToTarget(CREATURETYPE creatureType) {
	switch (creatureType)
	{
	case ANGLERFISH:
		return 2.5f;
	case SAWFISH:
		return 1.0f;
	default:
		return 0.0f;
	}
}

struct Pathfinder
{
	float minimumDistanceToTarget = 0.0f;
	float max_speed = 2.0f;
};