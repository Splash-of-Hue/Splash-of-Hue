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
/* isCreatureWithinColourRadius(Entity& creature, float elapsed_ms)
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
}*/

// TODO: https://trello.com/c/DMWehWpC/119-2-creaturebehavioursystem-hascreaturecollided
BEHAVIOUR_TREE_STATUS hasCreatureCollided(Entity& creature, float elapsed_ms)
{
	if (registry.collisions.has(creature)) 
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isCreatureInBlueRadius(Entity& creature, float elapsed_ms)
{
	if (checkIfInColourRadius(creature, COLOUR_STATE::BLUE)) {
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isCreatureInRedRadius(Entity& creature, float elapsed_ms)
{
	if (checkIfInColourRadius(creature, COLOUR_STATE::RED)) {
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isCreatureWithinPlayerColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.collisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	if (!registry.entitiesToLookAt.has(creature))
	{
		registry.entitiesToLookAt.emplace(creature);
	}

	EntityToLookAt& other_entity_component = registry.entitiesToLookAt.get(creature);
	Collision& creature_collision_component = registry.collisions.get(creature);
	for (auto it = creature_collision_component.collisionList.begin(); it != creature_collision_component.collisionList.end(); ++it)
	{
		if (registry.colourCircles.has(it->second))
		{
			ColourCircle& colour_circle = registry.colourCircles.get(it->second);
			Entity& source_entity = colour_circle.sourceEntity;
			if (registry.players.has(source_entity))
			{
				other_entity_component.entity = it->second;
				return BEHAVIOUR_TREE_STATUS::SUCCESS;
			}
		}
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS isCreatureWithinCreatureColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.collisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	if (!registry.entitiesToLookAt.has(creature))
	{
		registry.entitiesToLookAt.emplace(creature);
	}

	EntityToLookAt& other_entity_component = registry.entitiesToLookAt.get(creature);
	Collision& creature_collision_component = registry.collisions.get(creature);
	for (auto it = creature_collision_component.collisionList.begin(); it != creature_collision_component.collisionList.end(); ++it)
	{
		if (registry.colourCircles.has(it->second))
		{
			ColourCircle& colour_circle = registry.colourCircles.get(it->second);
			Entity& source_entity = colour_circle.sourceEntity;
			if (registry.creatures.has(source_entity))
			{
				other_entity_component.entity = it->second;
				return BEHAVIOUR_TREE_STATUS::SUCCESS;
			}
		}
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}
BEHAVIOUR_TREE_STATUS isCreatureWithinInteractablesColourRadius(Entity& creature, float elapsed_ms)
{
	if (!registry.collisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	if (!registry.entitiesToLookAt.has(creature))
	{
		registry.entitiesToLookAt.emplace(creature);
	}

	EntityToLookAt& other_entity_component = registry.entitiesToLookAt.get(creature);
	Collision& creature_collision_component = registry.collisions.get(creature);
	for (auto it = creature_collision_component.collisionList.begin(); it != creature_collision_component.collisionList.end(); ++it)
	{
		if (registry.colourCircles.has(it->second))
		{
			ColourCircle& colour_circle = registry.colourCircles.get(it->second);
			Entity& source_entity = colour_circle.sourceEntity;
			if (registry.interactables.has(source_entity))
			{
				other_entity_component.entity = it->second;
				return BEHAVIOUR_TREE_STATUS::SUCCESS;
			}
		}
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

bool checkIfInColourRadius(Entity& creature, COLOUR_STATE colour_state)
{
	if (!registry.collisions.has(creature))
	{
		false;
	}

	EntityToLookAt& other_entity_component = registry.entitiesToLookAt.get(creature);
	Entity& other_entity = other_entity_component.entity;
	Collision& creature_collision_component = registry.collisions.get(creature);

	if (creature_collision_component.collisionList.count(other_entity.id()) && registry.colourCircles.has(other_entity))
	{
		ColourState& other_colour_state = registry.colourStates.get(other_entity);
		if (other_colour_state.curr_colour_state == colour_state)
		{
			return true;
		}
	}

	return false;
}
