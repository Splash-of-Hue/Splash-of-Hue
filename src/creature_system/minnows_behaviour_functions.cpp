// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

// todo BukbklVV
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::updateMinnowsBlue(Entity& creature, float elapsed_ms)
{
	BoidOverseer& boid_overseer = registry.boidOverseers.get(creature);
	boid_overseer.alignment_factor = MINNOW_BLUE_ALIGNMENT_FACTOR;
	boid_overseer.cohesion_factor = MINNOW_BLUE_COHESION_FACTOR;
	boid_overseer.separation_factor = MINNOW_BLUE_SEPARATION_FACTOR;

	boid_overseer.target_factor = MINNOW_BLUE_TARGET_FACTOR;
	boid_overseer.target_position = registry.positions.get(registry.players.entities[0]);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::updateMinnowsNeutral(Entity& creature, float elapsed_ms)
{
	BoidOverseer& boid_overseer = registry.boidOverseers.get(creature);
	boid_overseer.alignment_factor = MINNOW_NEUTRAL_ALIGNMENT_FACTOR;
	boid_overseer.cohesion_factor = MINNOW_NEUTRAL_COHESION_FACTOR;
	boid_overseer.separation_factor = MINNOW_NEUTRAL_SEPARATION_FACTOR;

	boid_overseer.target_factor = MINNOW_NEUTRAL_TARGET_FACTOR;
	/*
		boid_overseer.alignment_factor = 0.02f;
	boid_overseer.cohesion_factor = 0.08f;
	boid_overseer.separation_factor = 5.5f;

	boid_overseer.target_factor = 0.0f;
	*/
	boid_overseer.target_position = registry.positions.get(registry.players.entities[0]);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::updateMinnowsRed(Entity& creature, float elapsed_ms)
{
	BoidOverseer& boid_overseer = registry.boidOverseers.get(creature);
	boid_overseer.alignment_factor = MINNOW_RED_ALIGNMENT_FACTOR;
	boid_overseer.cohesion_factor = MINNOW_RED_COHESION_FACTOR;
	boid_overseer.separation_factor = MINNOW_RED_SEPARATION_FACTOR;

	boid_overseer.target_factor = MINNOW_RED_TARGET_FACTOR;
	boid_overseer.target_position = registry.positions.get(registry.players.entities[0]);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}