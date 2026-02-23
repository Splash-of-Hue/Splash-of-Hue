// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_functions.hpp"

void updateAnglerfish(float elapsed_ms, Entity& anglerfish_entity)
{
	// Initialize components
	if (!registry.pathfinders.has(anglerfish_entity))
	{
		Pathfinder& pathfinder = registry.pathfinders.emplace(anglerfish_entity);
		pathfinder.minimumDistanceToTarget = pathfinderMinDistToTarget(ANGLERFISH);
		pathfinder.max_speed = registry.velocities.get(anglerfish_entity).x;
	}

	if (!registry.colourStates.has(anglerfish_entity))
	{
		ColourState& anglerfishColourState = registry.colourStates.emplace(anglerfish_entity);
		anglerfishColourState.curr_colour_state = COLOUR_STATE::NEUTRAL;
	}

	ColourState& anglerfishColourState = registry.colourStates.get(anglerfish_entity);

	switch (anglerfishColourState.curr_colour_state)
	{
	case COLOUR_STATE::BLUE:
		blueAnglerfish(elapsed_ms, anglerfish_entity);
		break;

	case COLOUR_STATE::RED:
		redAnglerfish(elapsed_ms, anglerfish_entity);
		break;

	default: // COLOUR_STATE::NEUTRAL or an undefined state
		neutralAnglerfish(anglerfish_entity);
		break;
	}
}

// For the Anglerfish, the neutral state is just the initial state
void neutralAnglerfish(Entity& anglerfish_entity)
{
	// Stationary, with the initial light colour
	registry.velocities.get(anglerfish_entity) = { 0.0f, 0.0f };
}

void blueAnglerfish(float elapsed_ms, Entity& anglerfish_entity)
{
	// Lights radius around it in blue, pathfinds to player
	follow_player(elapsed_ms, anglerfish_entity);
}

void redAnglerfish(float elapsed_ms, Entity& anglerfish_entity)
{
	// lights radius around it in red, pathfinds to player
	follow_player(elapsed_ms, anglerfish_entity);
}
