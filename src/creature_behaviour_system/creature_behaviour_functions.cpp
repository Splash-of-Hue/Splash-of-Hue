// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

#include <iostream>

BEHAVIOUR_TREE_STATUS stopCreatureMovement(Entity& creature, float elapsed_ms)
{
	if (!registry.velocities.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	vec2& creature_velocity = registry.velocities.get(creature);
	creature_velocity = { 0.0f, 0.0f };

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}
