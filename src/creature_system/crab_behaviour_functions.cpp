// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::slideForwards(Entity& creature, float elapsed_ms)
{
	vec2& velocity = registry.velocities.get(creature);
	vec2& direction = registry.directions.get(creature);

	velocity = vec2{ direction.x * (PLAYER_SPEED), direction.y * (PLAYER_SPEED) };

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::slideBackwards(Entity& creature, float elapsed_ms)
{
	vec2& velocity = registry.velocities.get(creature);
	vec2& direction = registry.directions.get(creature);
	velocity = vec2{ direction.x * (-PLAYER_SPEED), direction.y * (-PLAYER_SPEED) };

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

// ====== Decorator Nodes ======
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isSliding(Entity& creature, float elapsed_ms)
{
	if (registry.velocities.get(creature).x != 0.f || registry.velocities.get(creature).y != 0.f) {
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}
