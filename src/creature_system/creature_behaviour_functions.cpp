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

	ColourState& creature_colour_state = registry.colourStates.get(creature);
	creature_colour_state.prev_colour_state = creature_colour_state.curr_colour_state;
	creature_colour_state.curr_colour_state = COLOUR_STATE::RED;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS makeBlue(Entity& creature, float elapsed_ms)
{
	if (!registry.colourStates.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	ColourState& creature_colour_state = registry.colourStates.get(creature);
	creature_colour_state.prev_colour_state = creature_colour_state.curr_colour_state;
	creature_colour_state.curr_colour_state = COLOUR_STATE::BLUE;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS makeNeutral(Entity& creature, float elapsed_ms)
{
	if (!registry.colourStates.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	ColourState& creature_colour_state = registry.colourStates.get(creature);
	creature_colour_state.prev_colour_state = creature_colour_state.curr_colour_state;
	creature_colour_state.curr_colour_state = COLOUR_STATE::NEUTRAL;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS stopColliding(Entity& creature, float elapsed_ms)
{
	if (registry.collisions.has(creature)) {
		Collision& creature_collision_component = registry.collisions.get(creature);
		Entity& other_entity = creature_collision_component.other;
		
		if (registry.collisions.has(other_entity))
		{
			registry.collisions.remove(other_entity);
			
		}
		registry.collisions.remove(other_entity);
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
		
	}
		


	return BEHAVIOUR_TREE_STATUS::FAILURE;
}