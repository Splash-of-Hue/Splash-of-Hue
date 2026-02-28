// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

#include <iostream>

/*

SHRIMP-SPECIFIC BEHAVIOUR FUNCTIONS WILL GO HERE

Trello CRvulzMB

*/

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::disableCollidableComponent(std::vector<Entity>& entityList)
{
	for (Entity& entity : entityList)
	{
		if (!registry.collidables.has(entity) || !registry.colors.has(entity))
		{
			return BEHAVIOUR_TREE_STATUS::FAILURE;
		}

		Collidable& collidable = registry.collidables.get(entity);
		collidable.overlay = false;
		collidable.inelastic = false;
		
		vec4& colour = registry.colors.get(entity);
		colour.a = 0.0f; // set invisible
	}

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::enableCollidableComponent(std::vector<Entity>& entityList)
{
	for (Entity& entity : entityList)
	{
		if (!registry.collidables.has(entity) || !registry.colors.has(entity))
		{
			return BEHAVIOUR_TREE_STATUS::FAILURE;
		}

		Collidable& collidable = registry.collidables.get(entity);
		collidable.inelastic = true;

		vec4& colour = registry.colors.get(entity);
		colour.a = 1.0f; // set visible
	}

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::setShrimpArms(Entity& creature, SHRIMP_ARM_COLOUR shrimp_arm_colour)
{
	if (!registry.shrimps.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	Shrimp& shrimp_component = registry.shrimps.get(creature);

	for (auto& [colour, arms] : shrimp_component.shrimp_arms)
	{
		if (colour == shrimp_arm_colour)
		{
			BEHAVIOUR_TREE_STATUS status_result = enableCollidableComponent(arms);

			if (status_result == BEHAVIOUR_TREE_STATUS::FAILURE)
				return status_result;
		}
		else
		{
			BEHAVIOUR_TREE_STATUS status_result = disableCollidableComponent(arms);

			if (status_result == BEHAVIOUR_TREE_STATUS::FAILURE)
				return status_result;
		}
	}

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::neutralArms(Entity& creature, float elapsed_ms)
{
	// todo: 61gdK0v8 animation trigger?
	return setShrimpArms(creature, SHRIMP_ARM_COLOUR::NEUTRAL);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::blueArms(Entity& creature, float elapsed_ms)
{
	// todo: 61gdK0v8 animation trigger?
	return setShrimpArms(creature, SHRIMP_ARM_COLOUR::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::redArms(Entity& creature, float elapsed_ms)
{
	// todo: 61gdK0v8 animation trigger?
	return setShrimpArms(creature, SHRIMP_ARM_COLOUR::RED);
}
