// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

void CreatureBehaviourSystem::updateShrimp(float elapsed_ms)
{
	// get colour state based on Abby's work
	// based on that choose the corresponding function

	// TEMPORARY FOR TESTING
	if (registry.shrimps.entities.size() == 0)
	{
		Entity& shrimp_entity = Entity();
		registry.shrimps.emplace(shrimp_entity);

		Pathfinder& pathfinder = registry.pathfinders.emplace(shrimp_entity);

		Motion& motion = registry.motions.emplace(shrimp_entity);
		motion.position = { -100, -100 };
		motion.velocity = { 1.0, 1.0 };
	}

	follow_player(elapsed_ms, registry.shrimps.entities[0]);
}

// For the shrimp, the neutral state is the initial state
void CreatureBehaviourSystem::neutralShrimp()
{

}

void CreatureBehaviourSystem::blueShrimp()
{
	// punches Up & Right
	// switching depends on if it's touched again w blue

	// Up: should create an arm that's not collidable above the shrimp 
}

void CreatureBehaviourSystem::redShrimp()
{
	// punches left and down
	// switching depends on if its touched again w red
}