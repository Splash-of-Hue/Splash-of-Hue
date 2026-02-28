// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
#include <iostream>

// todo: https://trello.com/c/D5ie5Ml8/120-task-score-task-template
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::aimAtTarget(Entity& charger, float elapsed_ms)
{
	if (!registry.targets.has(charger))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	Target& charger_target_component = registry.targets.get(charger);
	Entity& targeted_entity = charger_target_component.entity;

	vec2 charger_position = registry.positions.get(charger);
	vec2 targeted_entity_position = registry.positions.get(targeted_entity);

	Angle& charger_angle = registry.angles.get(charger);
	
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::chargeAtTarget(Entity& charger, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::beDizzy(Entity& charger, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}