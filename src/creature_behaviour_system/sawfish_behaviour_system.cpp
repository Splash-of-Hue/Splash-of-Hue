// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"
#include "world_init.hpp" // For testing = using createrectangle

void CreatureBehaviourSystem::updateSawfish(float elapsed_ms, Entity& sawfish_entity)
{
	// Initialize components
	if (!registry.pathfinders.has(sawfish_entity))
	{
		Pathfinder& pathfinder = registry.pathfinders.emplace(sawfish_entity);
		pathfinder.minimumDistanceToTarget = pathfinderMinDistToTarget(SAWFISH);
		pathfinder.max_speed = registry.velocities.get(sawfish_entity).x;
	}

	if (!registry.colourStates.has(sawfish_entity))
	{
		ColourState& sawfishColourState = registry.colourStates.emplace(sawfish_entity);
		sawfishColourState.curr_colour_state = COLOUR_STATE::NEUTRAL;
	}

	ColourState& sawfishColourState = registry.colourStates.get(sawfish_entity);

	switch (sawfishColourState.curr_colour_state)
	{
	case COLOUR_STATE::BLUE:
		blueSawfish(sawfish_entity);
		break;

	case COLOUR_STATE::RED:
		redSawfish(elapsed_ms, sawfish_entity);
		break;

	default: // COLOUR_STATE::NEUTRAL or an undefined state
		neutralSawfish(elapsed_ms, sawfish_entity);
		break;
	}
}

void CreatureBehaviourSystem::neutralSawfish(float elapsed_ms, Entity& sawfish_entity)
{
	// aimlessly pathfinding. For M1, stationary

	// For testing only:
	follow_player(elapsed_ms, sawfish_entity);
}

void CreatureBehaviourSystem::blueSawfish(Entity& sawfish_entity)
{
	// stationary
	registry.velocities.get(sawfish_entity) = { 0.0f, 0.0f };
}

void CreatureBehaviourSystem::redSawfish(float elapsed_ms, Entity& sawfish_entity)
{
	// charges after the player
	follow_player(elapsed_ms, sawfish_entity);
}