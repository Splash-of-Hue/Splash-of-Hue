// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"
#include "world_init.hpp" // For testing = using createrectangle

void CreatureBehaviourSystem::updateSawfish(float elapsed_ms)
{
	// TEMPORARY FOR TESTING
	if (registry.shrimps.entities.size() == 0)
	{
		vec2 top_left = vec2(WINDOW_WIDTH_PX * 0.25f, WINDOW_HEIGHT_PX * 0.25f);
		vec2 bottom_right = vec2(WINDOW_WIDTH_PX * 0.5f, WINDOW_HEIGHT_PX * 0.50f);

		Entity& shrimp_entity = createRectangle(top_left, bottom_right, vec4(0.2f, 0.2f, 0.2f, 1));
		registry.rectangles.emplace(shrimp_entity);
		registry.shrimps.emplace(shrimp_entity);

		Pathfinder& pathfinder = registry.pathfinders.emplace(shrimp_entity);

		Motion& motion = registry.motions.get(shrimp_entity);
		//motion.position = { -10, -50 };
		//motion.velocity = { 1.0, 1.0 };

	}

	follow_player(elapsed_ms, registry.shrimps.entities[0]);
}

void CreatureBehaviourSystem::neutralSawfish()
{
	// aimlessly pathfinding
}

void CreatureBehaviourSystem::blueSawfish()
{
	// stationary
}

void CreatureBehaviourSystem::redSawfish()
{
	// charges after the player
}