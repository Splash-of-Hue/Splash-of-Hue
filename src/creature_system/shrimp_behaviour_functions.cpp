// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

/*

SHRIMP-SPECIFIC BEHAVIOUR FUNCTIONS WILL GO HERE

Trello CRvulzMB

*/

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::disableCollidableComponent(std::vector<Entity>& entityList)
{
	for (Entity& entity : entityList)
	{
		if (registry.collidables.has(entity))
		{
			Collidable& collidable = registry.collidables.get(entity);
			collidable.overlay = false;
			collidable.inelastic = false;
		}
		else
		{
			return BEHAVIOUR_TREE_STATUS::FAILURE;
		}
	}

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::enableCollidableComponent(std::vector<Entity>& entityList)
{
	for (Entity& entity : entityList)
	{
		if (registry.collidables.has(entity))
		{
			Collidable& collidable = registry.collidables.emplace(entity);
			collidable.inelastic = true;
		}
		else
		{
			return BEHAVIOUR_TREE_STATUS::FAILURE;
		}
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
			enableCollidableComponent(arms);
		else
			disableCollidableComponent(arms);
	}
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::neutralArms(Entity& creature, float elapsed_ms)
{
	// animation trigger?

	return setShrimpArms(creature, SHRIMP_ARM_COLOUR::NEUTRAL);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::blueArms(Entity& creature, float elapsed_ms)
{
	// animation trigger?

	return setShrimpArms(creature, SHRIMP_ARM_COLOUR::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::redArms(Entity& creature, float elapsed_ms)
{
	// animation trigger?

	return setShrimpArms(creature, SHRIMP_ARM_COLOUR::RED);
}




