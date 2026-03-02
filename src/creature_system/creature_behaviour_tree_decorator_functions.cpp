#pragma once

#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

#include <iostream>

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isInBlueState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);

	if (colourState.curr_colour_state == COLOUR_STATE::BLUE)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isInRedState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);

	if (colourState.curr_colour_state == COLOUR_STATE::RED)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isInNeutralState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);

	if (colourState.curr_colour_state == COLOUR_STATE::NEUTRAL)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

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

// TODO: https://trello.com/c/9KLJ5MXH/118-2-creature-behaviour-system-iscreaturewithincolourradius
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinAnyColourRadius(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

// TODO: https://trello.com/c/lw7cUBLv/141-2creaturebehavioursystem-iscreaturewithinplayercolourradius
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinPlayerColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.inPlayerRadius.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

// TODO: https://trello.com/c/lw7cUBLv/141-2creaturebehavioursystem-iscreaturewithinplayercolourradius
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinBluePlayerColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.inPlayerRadius.has(creature))
	{
		InPlayerRadius& in_player_radius_component = registry.inPlayerRadius.get(creature);
		Entity& radius_entity = in_player_radius_component.radius_entity;

		ColourState& radius_colour_state = registry.colourStates.get(radius_entity);

		if (radius_colour_state.curr_colour_state == COLOUR_STATE::BLUE)
		{
			return BEHAVIOUR_TREE_STATUS::SUCCESS;
		}
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinRedPlayerColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.inPlayerRadius.has(creature))
	{
		InPlayerRadius& in_player_radius_component = registry.inPlayerRadius.get(creature);
		Entity& radius_entity = in_player_radius_component.radius_entity;

		ColourState& radius_colour_state = registry.colourStates.get(radius_entity);

		if (radius_colour_state.curr_colour_state == COLOUR_STATE::RED)
		{
			return BEHAVIOUR_TREE_STATUS::SUCCESS;
		}
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

// TODO: https://trello.com/c/V8KcHxHE/142-2-creature-behaviour-system-iscreaturewithincreaturecolourradius
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinCreatureColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.inCreatureRadius.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

// TODO: https://trello.com/c/aIFrZZcy/143-2-creature-behaviour-system-iscreaturewithininteractablecolourradius
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinInteractableColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.inInteractableRadius.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

// TODO: https://trello.com/c/DMWehWpC/119-2-creaturebehavioursystem-hascreaturecollided
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::hasCreatureCollided(Entity& creature, float elapsed_ms)
{
	if (registry.creatureInelasticCollisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::RUNNING;
}

// TODO: https://trello.com/c/DMWehWpC/119-2-creaturebehavioursystem-hascreaturecollided
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::hasCooldown(Entity& creature, float elapsed_ms)
{
	if (registry.creatureBehaviourCooldowns.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCharging(Entity& creature, float elapsed_ms)
{
	if (registry.chargers.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isDizzy(Entity& creature, float elapsed_ms)
{
	if (registry.dizzies.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}
