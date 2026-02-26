#pragma once

#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

#include <iostream>

BEHAVIOUR_TREE_STATUS isInBlueState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);

	if (colourState.curr_colour_state == COLOUR_STATE::BLUE)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isInRedState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);

	if (colourState.curr_colour_state == COLOUR_STATE::RED)
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	else
		return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isInNeutralState(Entity& creature, float elapsed_ms)
{
	ColourState& colourState = registry.colourStates.get(creature);

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
	if (registry.collisions.has(creature))
	{
		Collision& creature_collision_component = registry.collisions.get(creature);
		Entity& circle_entity = creature_collision_component.other;
		if (registry.colourCircles.has(circle_entity))
		{
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

// TODO: https://trello.com/c/DMWehWpC/119-2-creaturebehavioursystem-hascreaturecollided
BEHAVIOUR_TREE_STATUS hasCreatureCollided(Entity& creature, float elapsed_ms)
{
	if (registry.collisions.has(creature)) 
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isCreatureWithinPlayerColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.collisions.has(creature)) 
	{
		Collision& creature_collision_component = registry.collisions.get(creature);
		Entity& circle_entity = creature_collision_component.other;
		if (registry.colourCircles.has(circle_entity))
		{
			ColourCircle& circle_component = registry.colourCircles.get(circle_entity);
			Entity& source_entity = circle_component.sourceEntity;
			
			if (registry.players.has(source_entity)) {
				return BEHAVIOUR_TREE_STATUS::SUCCESS;
			}
		}
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isCreatureWithinCreatureColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.collisions.has(creature))
	{
		Collision& creature_collision_component = registry.collisions.get(creature);
		Entity& circle_entity = creature_collision_component.other;
		if (registry.colourCircles.has(circle_entity))
		{
			ColourCircle& circle_component = registry.colourCircles.get(circle_entity);
			Entity& source_entity = circle_component.sourceEntity;

			if (registry.creatures.has(source_entity)) {
				return BEHAVIOUR_TREE_STATUS::SUCCESS;
			}
		}
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isCreatureWithinInteractableColourRadius(Entity& creature, float elapsed_ms)
{
	if (registry.collisions.has(creature))
	{
		Collision& creature_collision_component = registry.collisions.get(creature);
		Entity& circle_entity = creature_collision_component.other;
		if (registry.colourCircles.has(circle_entity))
		{
			ColourCircle& circle_component = registry.colourCircles.get(circle_entity);
			Entity& source_entity = circle_component.sourceEntity;

			if (registry.interactables.has(source_entity)) {
				return BEHAVIOUR_TREE_STATUS::SUCCESS;
			}
		}
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}
