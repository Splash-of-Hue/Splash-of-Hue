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

	std::cout << "I am Aiming!" << std::endl;
	Entity& player_entity = registry.players.entities[0];


	vec2& charger_position = registry.positions.get(charger);
	vec2& targeted_entity_position = registry.positions.get(player_entity);

	Angle& charger_angle = registry.angles.get(charger);
	
	float degAngle = atan((charger_position.x - targeted_entity_position.x) / (targeted_entity_position.y - charger_position.y)) * 180 / 3.14;
	if (targeted_entity_position.y - charger_position.y < 0) {
		degAngle = -90 + degAngle;
	}
	else {
		degAngle = 90 + degAngle;
	}
	charger_angle.degrees = degAngle;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::sawfishChargeAtTarget(Entity& charger, float elapsed_ms)
{
	std::cout << "Charging! 1" << std::endl;
	Target& charger_target_component = registry.targets.get(charger);
	Entity& targeted_entity = charger_target_component.entity;
	std::cout << "Charging! 2" << std::endl;
	vec2& charger_position = registry.positions.get(charger);
	vec2& targeted_entity_position = registry.positions.get(targeted_entity);
	std::cout << "Charging! 3" << std::endl;
	vec2 velocity = { targeted_entity_position.x - charger_position.x,targeted_entity_position.y - charger_position.y };
	double mag = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	velocity *= (1.f / mag);
	std::cout << "Charging! 4" << std::endl;
	vec2& charger_velocity = registry.velocities.get(charger);
	charger_velocity = velocity;
	std::cout << "Charging! 5" << std::endl;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::beDizzy(Entity& charger, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}