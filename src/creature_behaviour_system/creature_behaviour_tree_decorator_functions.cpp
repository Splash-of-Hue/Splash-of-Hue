#pragma once

#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_functions.hpp"

#include <iostream>

BEHAVIOUR_TREE_STATUS isInBlueState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);
	std::cout << "isInBlueState Current colour state: " << static_cast<int>(colourState.curr_colour_state) << std::endl;

	if (colourState.curr_colour_state == COLOUR_STATE::BLUE)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isInRedState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);
	std::cout << "isInRedState Current colour state: " << static_cast<int>(colourState.curr_colour_state) << std::endl;

	if (colourState.curr_colour_state == COLOUR_STATE::RED)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isInNeutralState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);
	std::cout << "isInNeutralState Current colour state: " << static_cast<int>(colourState.curr_colour_state) << std::endl;

	if (colourState.curr_colour_state == COLOUR_STATE::NEUTRAL)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

// Will START executing behaviour after cooldown is completed
BEHAVIOUR_TREE_STATUS isCooldownComplete(Entity& creature, float elapsed_ms)
{
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
BEHAVIOUR_TREE_STATUS isTimerComplete(Entity& creature, float elapsed_ms)
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
BEHAVIOUR_TREE_STATUS isCreatureWithinColourRadius(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

// TODO: https://trello.com/c/DMWehWpC/119-2-creaturebehavioursystem-hascreaturecollided
BEHAVIOUR_TREE_STATUS hasCreatureCollided(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}
