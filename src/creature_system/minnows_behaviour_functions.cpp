// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

// todo BukbklVV
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::updateMinnowsBlue(Entity& creature, float elapsed_ms)
{
	BoidOverseer& boid_overseer = registry.boidOverseers.get(creature);

	boid_overseer.alignment_factor = 0.002f;
	boid_overseer.cohesion_factor = 5.5f;
	boid_overseer.separation_factor = 10.5f;
	boid_overseer.target_factor = 10.0f;
	boid_overseer.target_position = registry.positions.get(registry.players.entities[0]);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::updateMinnowsNeutral(Entity& creature, float elapsed_ms)
{
	//BoidOverseer& boid_overseer = registry.boidOverseers.get(creature);

	//boid_overseer.alignment_factor = 0.06f;
	//boid_overseer.cohesion_factor = 0.8f;
	//boid_overseer.separation_factor = 5.5f;

	//float max_speed = 100.0f;
	//float max_force = 20.0f;

	//boid_overseer.target_factor = 0.0;
	//boid_overseer.target_position = registry.positions.get(registry.players.entities[0]);

	BoidOverseer& boid_overseer = registry.boidOverseers.get(creature);
	boid_overseer.alignment_factor = 0.02f;
	boid_overseer.cohesion_factor = 0.08f;
	boid_overseer.separation_factor = 5.5f;

	boid_overseer.target_factor = 0.0f;
	boid_overseer.target_position = registry.positions.get(registry.players.entities[0]);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::updateMinnowsRed(Entity& creature, float elapsed_ms)
{
	BoidOverseer& boid_overseer = registry.boidOverseers.get(creature);
	boid_overseer.alignment_factor = 0.08f;
	boid_overseer.cohesion_factor = 0.001f;
	boid_overseer.separation_factor = 25.0f;

	boid_overseer.target_factor = 0.1f;
	boid_overseer.target_position = registry.positions.get(registry.players.entities[0]);

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}