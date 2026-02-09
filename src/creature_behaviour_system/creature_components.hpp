#pragma once
#include "creature_behaviour_system/creature_constants.hpp"

struct Creature
{
	CREATURETYPE creatureType = UNDEFINED;
	// colour type here from Abby's colour system
};

struct Shrimp
{
	// Tracks current direction shrimp is facing.
	DIRECTION currDirect = UP;
};

struct Pathfinder
{
	float minimumDistanceToTarget = 0.0f;
	float max_speed = 2.0f;
};
