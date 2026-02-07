// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"
#include "world_init.hpp" // For testing = using createrectangle

void CreatureBehaviourSystem::updateSawfish(float elapsed_ms, Entity& sawfish_entity, Creature& creature_component)
{
	assert(registry.motions.has(sawfish_entity) && "sawfish does not have a motion component");
	if (!registry.pathfinders.has(sawfish_entity))
	{
		registry.pathfinders.emplace(sawfish_entity);
	}

	follow_player(elapsed_ms, sawfish_entity);
}

void CreatureBehaviourSystem::neutralSawfish()
{
	// aimlessly pathfinding
}

void CreatureBehaviourSystem::blueSawfish()
{
	// stationary
}

void CreatureBehaviourSystem::redSawfish()
{
	// charges after the player
}