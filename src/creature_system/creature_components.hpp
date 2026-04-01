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

struct IsInInelasticCollision
{

};

//Sawfish-specific components
struct Charging
{

};

struct Dizzy
{

};
