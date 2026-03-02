#pragma once
#include "common.hpp"
#include "creature_system/creature_behaviour_tree_constants.hpp"
#include "creature_system/creature_constants.hpp"
#include <ai/ai_constants.hpp>
#include <unordered_map>
#include <vector>

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
	int num_arms = 2; // in the future, shrimp will be able to be initialized for 1, 2, or 3 arms. Trello: JZSDWzTQ
	std::unordered_map<SHRIMP_ARM_COLOUR, std::vector<Entity>> shrimp_arms;
};

struct Pathfinder
{
	float minimum_distance_to_target = 70.0f;
	float max_speed = 2.0f;
	// The type of pathfinding. If this is changed, the path will change
	PATHFINDING_TYPE type;
	// A boolean about whether the path to the target needs updating
	bool is_path_dirty = true;
	// A set of points that makes up the map
	std::vector<vec2> path;
	// The thing to follow. The target is assumed to have a position
	Entity target;
};

struct Charging
{

};

struct IsInInelasticCollision
{

};

struct Dizzy
{

};