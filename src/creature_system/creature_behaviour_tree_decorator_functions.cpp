#pragma once

#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

#include <iostream>

//  ===== Colour Decorator Nodes =====
//todo: https://trello.com/c/SPoplE91/184-1-consolidate-inxradius-components
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinAnyColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.inRadius.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	InRadius& in_radius_component = registry.inRadius.get(creature);
	
	if (in_radius_component.inPlayerRadius || in_radius_component.inCreatureRadius || in_radius_component.inInteractableRadius)
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinSpecificColourRadius(Entity& creature, bool& inRadius, std::vector<Entity>& radius_list, COLOUR_STATE colour_state)
{
	if (!inRadius)
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	for (Entity& entity : radius_list)
	{
		if (!registry.colourStates.has(entity))
		{
			return BEHAVIOUR_TREE_STATUS::FAILURE;
		}

		ColourState& radius_colour_state = registry.colourStates.get(entity);

		if (radius_colour_state.curr_colour_state == colour_state)
		{
			return BEHAVIOUR_TREE_STATUS::SUCCESS;
		}
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinPlayerBlueColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.inRadius.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	InRadius& in_radius_component = registry.inRadius.get(creature);
	return isCreatureWithinSpecificColourRadius(creature, in_radius_component.inPlayerRadius, in_radius_component.playerRadiusList, COLOUR_STATE::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinPlayerRedColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.inRadius.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	InRadius& in_radius_component = registry.inRadius.get(creature);
	return isCreatureWithinSpecificColourRadius(creature, in_radius_component.inPlayerRadius, in_radius_component.playerRadiusList, COLOUR_STATE::RED);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinCreatureBlueColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.inRadius.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	InRadius& in_radius_component = registry.inRadius.get(creature);
	return isCreatureWithinSpecificColourRadius(creature, in_radius_component.inCreatureRadius, in_radius_component.creatureRadiusList, COLOUR_STATE::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinCreatureRedColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.inRadius.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	InRadius& in_radius_component = registry.inRadius.get(creature);
	return isCreatureWithinSpecificColourRadius(creature, in_radius_component.inCreatureRadius, in_radius_component.creatureRadiusList, COLOUR_STATE::RED);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinInteractableBlueColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.inRadius.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	InRadius& in_radius_component = registry.inRadius.get(creature);
	return isCreatureWithinSpecificColourRadius(creature, in_radius_component.inInteractableRadius, in_radius_component.interactableRadiusList, COLOUR_STATE::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinInteractableRedColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.inRadius.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	InRadius& in_radius_component = registry.inRadius.get(creature);
	return isCreatureWithinSpecificColourRadius(creature, in_radius_component.inInteractableRadius, in_radius_component.interactableRadiusList, COLOUR_STATE::RED);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isInBlueState(Entity& creature, float elapsed_ms)
{
	if (registry.colourStates.get(creature).curr_colour_state == COLOUR_STATE::BLUE)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isInNeutralState(Entity& creature, float elapsed_ms)
{
	if (registry.colourStates.get(creature).curr_colour_state == COLOUR_STATE::NEUTRAL)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isInRedState(Entity& creature, float elapsed_ms)
{
	if (registry.colourStates.get(creature).curr_colour_state == COLOUR_STATE::RED)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

//  ===== Movement Decorator Nodes =====
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::hasCreatureCollided(Entity& creature, float elapsed_ms)
{
	if (registry.creatureInelasticCollisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::RUNNING;
}

//  ===== Miscellaneous Decorator Nodes =====
// Will START executing behaviour after cooldown is completed
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCooldownComplete(Entity& creature, float elapsed_ms)
{
	if (!registry.creatureBehaviourCooldowns.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
	CreatureBehaviourCooldown& cooldown = registry.creatureBehaviourCooldowns.get(creature);
	if (cooldown.cooldown <= 0)
	{
		cooldown.cooldown = cooldown.cooldown_default; // reset cooldown
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	else
	{
		cooldown.cooldown -= elapsed_ms;
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
}

// Will STOP executing behaviour after timer is completed
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isTimerComplete(Entity& creature, float elapsed_ms)
{
	CreatureBehaviourTimer& timer = registry.creatureBehaviourTimers.get(creature);
	if (timer.timer <= 0)
	{
		timer.timer = timer.timer_default; // reset timer
		return BEHAVIOUR_TREE_STATUS::FAILURE; // stop executing behaviour
	}
	else
	{
		timer.timer -= elapsed_ms;
		return BEHAVIOUR_TREE_STATUS::SUCCESS; // continue executing behaviour
	}
}
