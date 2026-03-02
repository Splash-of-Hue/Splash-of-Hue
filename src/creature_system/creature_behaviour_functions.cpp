// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_system.hpp"

#include <iostream>

CreatureBehaviourFunctions creatureBehaviourFunctions;

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::stopCreatureMovement(Entity& creature, float elapsed_ms)
{
	if (!registry.velocities.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	vec2& creature_velocity = registry.velocities.get(creature);
	creature_velocity = { 0.0f, 0.0f };

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeNeutral(Entity& creature, float elapsed_ms)
{
	if (!registry.colourStates.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
	ColourState& colour_state_component = registry.colourStates.get(creature);
	colour_state_component.prev_colour_state = colour_state_component.prev_colour_state;
	colour_state_component.curr_colour_state = COLOUR_STATE::NEUTRAL;

	vec4& color = registry.colors.get(creature);
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(colour_state_component.curr_colour_state);
	color = color_from_state;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
	
}
