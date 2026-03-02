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
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeBlue(Entity& creature, float elapsed_ms)
{
	ColourState& colour_state_creature = registry.colourStates.get(creature);
	colour_state_creature.prev_colour_state = colour_state_creature.curr_colour_state;
	colour_state_creature.curr_colour_state = COLOUR_STATE::BLUE;

	vec4& other_color = registry.colors.get(creature);
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(COLOUR_STATE::BLUE);
	other_color = color_from_state;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeRed(Entity& creature, float elapsed_ms)
{
	ColourState& colour_state_creature = registry.colourStates.get(creature);
	colour_state_creature.prev_colour_state = colour_state_creature.curr_colour_state;
	colour_state_creature.curr_colour_state = COLOUR_STATE::RED;

	vec4& other_color = registry.colors.get(creature);
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(COLOUR_STATE::RED);
	other_color = color_from_state;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::applyCooldown(Entity& creature, float elapsed_ms)
{
	if (!registry.creatureBehaviourCooldowns.has(creature))
	{
		registry.creatureBehaviourCooldowns.emplace(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeCooldown(Entity& creature, float elapsed_ms)
{
	if (registry.creatureBehaviourCooldowns.has(creature))
	{
		registry.creatureBehaviourCooldowns.remove(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::applyCharging(Entity& creature, float elapsed_ms)
{
	if (!registry.chargers.has(creature))
	{
		registry.chargers.emplace(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::returnSuccess(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::returnFailure(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeCollision(Entity& creature, float elapsed_ms)
{
	if (registry.creatureInelasticCollisions.has(creature))
	{
		registry.creatureInelasticCollisions.remove(creature);
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

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeDizzy(Entity& creature, float elapsed_ms)
{
	if (registry.dizzies.has(creature))
	{
		registry.dizzies.remove(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}
