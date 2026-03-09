// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_system.hpp"

#include <iostream>

CreatureBehaviourFunctions creatureBehaviourFunctions;

// ===== Colour Action Nodes =====
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeColour(Entity& creature, COLOUR_STATE colour_state)
{
	if (!registry.colourStates.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
	ColourState& colour_state_component = registry.colourStates.get(creature);
	colour_state_component.prev_colour_state = colour_state_component.prev_colour_state;
	colour_state_component.curr_colour_state = colour_state;

	vec4& color = registry.colors.get(creature);
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(colour_state_component.curr_colour_state);
	color = color_from_state;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeBlue(Entity& creature, float elapsed_ms)
{
	return makeColour(creature, COLOUR_STATE::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeRed(Entity& creature, float elapsed_ms)
{
	return makeColour(creature, COLOUR_STATE::RED);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeNeutral(Entity& creature, float elapsed_ms)
{
	return makeColour(creature, COLOUR_STATE::NEUTRAL);
}

//  ===== Movement Action Nodes =====
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::chargeAtTarget(Entity& charger, float elapsed_ms)
{
	// TODO:
	// Currently, charging is only being used for the sawfish. The "sawfishChargeAtTarget" node should work
	// for creatures that are not the sawfish, so please test that and let Spencer know if it does not work. 

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::stopCreatureMovement(Entity& creature, float elapsed_ms)
{
	if (!registry.velocities.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	vec2& creature_velocity = registry.velocities.get(creature);
	creature_velocity = { 0.0f, 0.0f };

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeCollision(Entity& creature, float elapsed_ms)
{
	if (registry.creatureInelasticCollisions.has(creature))
	{
		registry.creatureInelasticCollisions.remove(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

//  ===== Miscellaneous Action Nodes =====
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

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::returnSuccess(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::returnFailure(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}
