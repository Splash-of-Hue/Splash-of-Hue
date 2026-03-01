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

// TODO: https://trello.com/c/DMWehWpC/119-2-creaturebehavioursystem-hascreaturecollided
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::hasCreatureCollided(Entity& creature, float elapsed_ms)
{
	if (registry.collisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureInBlueRadius(Entity& creature, float elapsed_ms)
{
	return checkIfInColourRadius(creature, COLOUR_STATE::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureInRedRadius(Entity& creature, float elapsed_ms)
{
	return checkIfInColourRadius(creature, COLOUR_STATE::RED);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinPlayerColourRadius(Entity& creature, float elapsed_ms)
{
	return checkIfInComponentRadius(creature, registry.players);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinCreatureColourRadius(Entity& creature, float elapsed_ms)
{
	return checkIfInComponentRadius(creature, registry.creatures);
}
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isCreatureWithinInteractableColourRadius(Entity& creature, float elapsed_ms)
{

	return checkIfInComponentRadius(creature, registry.interactables);
}

//helper functions
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::checkIfInColourRadius(Entity& creature, COLOUR_STATE colour_state)
{
	if (!registry.collisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	EntityToLookAt& light_emission_entity_component = registry.entitiesToLookAt.get(creature);
	Entity& light_emission_entity = light_emission_entity_component.entity;
	Collision& creature_collision_component = registry.collisions.get(creature);

	if (creature_collision_component.collisionList.count(light_emission_entity.id()) && registry.lightEmissions.has(light_emission_entity))
	{
		ColourState& light_emission_colour_state = registry.colourStates.get(light_emission_entity);
		if (light_emission_colour_state.curr_colour_state == colour_state)
		{
			return BEHAVIOUR_TREE_STATUS::SUCCESS;
		}
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

template <typename Component>
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::checkIfInComponentRadius(Entity& creature, Component& component_type)
{
	if (!registry.collisions.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	if (!registry.entitiesToLookAt.has(creature))
	{
		registry.entitiesToLookAt.emplace(creature);
	}

	EntityToLookAt& light_emission_entity_component = registry.entitiesToLookAt.get(creature);
	Collision& creature_collision_component = registry.collisions.get(creature);
	for (auto it = creature_collision_component.collisionList.begin(); it != creature_collision_component.collisionList.end(); ++it)
	{
		if (registry.lightEmissions.has(it->second))
		{
			LightEmission& light_emission = registry.lightEmissions.get(it->second);
			Entity& source_entity = light_emission.lightSourceEntity;
			if (component_type.has(source_entity))
			{
				std::cout << "Creature is in " << typeid(component_type).name() << " Radius" << std::endl;
				light_emission_entity_component.entity = it->second;
				registry.list_all_components_of(light_emission_entity_component.entity);
				return BEHAVIOUR_TREE_STATUS::SUCCESS;
			}
		}
	}

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

