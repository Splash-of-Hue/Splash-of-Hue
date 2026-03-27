// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
#include <iostream>

// ===== Action Nodes =====
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::applyCharging(Entity& creature, float elapsed_ms)
{
	if (!registry.chargers.has(creature))
	{
		registry.chargers.emplace(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeCharging(Entity& creature, float elapsed_ms)
{
	if (registry.chargers.has(creature))
	{
		registry.chargers.remove(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::applyDizzy(Entity& creature, float elapsed_ms)
{
	if (!registry.dizzies.has(creature))
	{
		registry.dizzies.emplace(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::beDizzy(Entity& creature, float elapsed_ms)
{
	if (!registry.dizzies.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
	Angle& charger_angle = registry.angles.get(creature);
	charger_angle.degrees += 5.f;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeDizzy(Entity& creature, float elapsed_ms)
{
	if (registry.dizzies.has(creature))
	{
		registry.dizzies.remove(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::aimAtTarget(Entity& charger, float elapsed_ms)
{

	Entity& player_entity = registry.players.entities[0];

	vec2& charger_position = registry.positions.get(charger);
	vec2& targeted_entity_position = registry.positions.get(player_entity);
	// Get angles for charger
	Angle& charger_angle = registry.angles.get(charger);
	// Get arctangent to go from vector to angle
	float degAngle = atan((charger_position.x - targeted_entity_position.x) / (targeted_entity_position.y - charger_position.y)) * 180 / M_PI;

	if (targeted_entity_position.y - charger_position.y < 0)
	{
		degAngle = -90 + degAngle;
	}
	else
	{
		degAngle = 90 + degAngle;
	}

	// TODO: Change later (no ticket)
	// Change angle so it looks like looking at target
	if (registry.velocities.get(charger).x < 0)
	{
		degAngle = -degAngle - 180;
	}

	charger_angle.degrees = degAngle;
	
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

// ====== Decorator Nodes ======
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCharging(Entity& creature, float elapsed_ms)
{
	if (registry.chargers.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isDizzy(Entity& creature, float elapsed_ms)
{
	if (registry.dizzies.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}
