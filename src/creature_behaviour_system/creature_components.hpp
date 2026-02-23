#pragma once
#include "creature_behaviour_system/creature_constants.hpp"
#include "creature_behaviour_system/creature_behaviour_tree_constants.hpp"

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

struct CreatureBehaviour
{
	BehaviourTree* behaviour_tree;
	std::unordered_map<int, int> currentChildNode;
};
