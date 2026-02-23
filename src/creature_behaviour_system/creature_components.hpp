#pragma once
#include "creature_behaviour_system/creature_constants.hpp"
#include "creature_behaviour_system/creature_behaviour_tree_constants.hpp"

struct Creature
{
	CREATURETYPE creatureType = UNDEFINED;
	// colour type here from Abby's colour system
};

struct CreatureBehaviour
{
	BehaviourTree behaviour_tree;
	std::unordered_map<int, int> currentChildNode;
};

struct CreatureBehaviourCooldown
{
	float cooldown = COOLDOWN_TIMER_DEFAULT;
	float cooldown_default = cooldown;
};

struct CreatureBehaviourTimer
{
	float timer = TIMELIMIT_TIMER_DEFAULT;
	float timer_default = 0;
};

struct Shrimp
{
	// Tracks current direction shrimp is facing.
	DIRECTION currDirect = UP;
};

struct Pathfinder
{
	float minimumDistanceToTarget = 70.0f;
	float max_speed = 2.0f;
};
