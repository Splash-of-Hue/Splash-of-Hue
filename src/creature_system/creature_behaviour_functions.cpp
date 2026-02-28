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

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeRed(Entity& creature, float elapsed_ms)
{
	std::cout << "Creature Is Turning RED!" << std::endl;
	return makeColour(creature, COLOUR_STATE::RED);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeBlue(Entity& creature, float elapsed_ms)
{
	std::cout << "Creature Is Turning Blue!" << std::endl;
	return makeColour(creature, COLOUR_STATE::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeNeutral(Entity& creature, float elapsed_ms)
{
	std::cout << "Creature Is Turning Neutral!" << std::endl;
	return makeColour(creature, COLOUR_STATE::NEUTRAL);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::stopColliding(Entity& creature, float elapsed_ms)
{
	if (!registry.collisions.has(creature) || !registry.entitiesToLookAt.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	EntityToLookAt& other_entity_component = registry.entitiesToLookAt.get(creature);
	Entity& other_entity = other_entity_component.entity;
	Collision& creature_collision_component = registry.collisions.get(creature);

	if (!creature_collision_component.collisionList.count(other_entity.id()))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	Collision& other_collision_component = registry.collisions.get(other_entity);

	other_collision_component.collisionList.erase(creature.id());
	creature_collision_component.collisionList.erase(other_entity.id());

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::setTargetToEmissionSource(Entity& creature, float elapsed_ms)
{
	EntityToLookAt& light_emission_entity_component = registry.entitiesToLookAt.get(creature);
	Entity& light_emission_entity = light_emission_entity_component.entity;
	
	if (!registry.lightEmissions.has(light_emission_entity))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
	
	if (!registry.targets.has(creature))
	{
		registry.targets.emplace(creature);

	}

	LightEmission& light_emission_component = registry.lightEmissions.get(light_emission_entity);
	Entity& light_source_entity = light_emission_component.lightSourceEntity;

	Target& target_component = registry.targets.get(creature);
	target_component.entity = light_source_entity;

	CreatureBehaviourCooldown& cooldown = registry.creatureBehaviourCooldowns.emplace(creature);

	registry.list_all_components_of(light_source_entity);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

//Helper Functions
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeColour(Entity& creature, COLOUR_STATE colour_state)
{
	if (!registry.colourStates.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	ColourState& creature_colour_state = registry.colourStates.get(creature);
	creature_colour_state.prev_colour_state = creature_colour_state.curr_colour_state;
	creature_colour_state.curr_colour_state = colour_state;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}
