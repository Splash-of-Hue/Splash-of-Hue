// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_system.hpp"

#include <iostream>

BEHAVIOUR_TREE_STATUS stopCreatureMovement(Entity& creature, float elapsed_ms)
{
	if (!registry.velocities.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	vec2& creature_velocity = registry.velocities.get(creature);
	creature_velocity = { 0.0f, 0.0f };

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS makeRed(Entity& creature, float elapsed_ms)
{
	if (!registry.colourStates.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	std::cout << "Creature Is Turning Red!" << std::endl;
	makeColour(creature, COLOUR_STATE::RED);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS makeBlue(Entity& creature, float elapsed_ms)
{
	if (!registry.colourStates.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	std::cout << "Creature Is Turning Blue!" << std::endl;
	makeColour(creature, COLOUR_STATE::BLUE);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS makeNeutral(Entity& creature, float elapsed_ms)
{
	if (!registry.colourStates.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	std::cout << "Creature Is Turning Neutral!" << std::endl;
	makeColour(creature, COLOUR_STATE::NEUTRAL);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS stopColliding(Entity& creature, float elapsed_ms)
{
	if (!registry.collisions.has(creature) || !registry.entitiesToLookAt.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	EntityToLookAt& other_entity_component = registry.entitiesToLookAt.get(creature);
	Entity& other_entity = other_entity_component.entity;
	Collision& creature_collision_component = registry.collisions.get(creature);

	if (!creature_collision_component.collisionList.count(other_entity.id()))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	Collision& other_collision_component = registry.collisions.get(other_entity);

	other_collision_component.collisionList.erase(creature.id());
	creature_collision_component.collisionList.erase(other_entity.id());
	
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

void makeColour(Entity& creature, COLOUR_STATE colour_state) 
{
	ColourState& creature_colour_state = registry.colourStates.get(creature);
	creature_colour_state.prev_colour_state = creature_colour_state.curr_colour_state;
	creature_colour_state.curr_colour_state = colour_state;
}
