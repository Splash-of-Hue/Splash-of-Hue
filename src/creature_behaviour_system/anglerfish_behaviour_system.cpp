// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

void CreatureBehaviourSystem::updateAnglerfish(float elapsed_ms, Entity& anglerfish_entity)
{
	if (!registry.colourStates.has(anglerfish_entity))
	{
		ColourState& anglerfishColourState = registry.colourStates.emplace(anglerfish_entity);
		anglerfishColourState.curr_colour_state = COLOUR_STATE::NEUTRAL;
	}

	ColourState& anglerfishColourState = registry.colourStates.get(anglerfish_entity);

	switch (anglerfishColourState.curr_colour_state)
	{
	case COLOUR_STATE::BLUE:
		blueAnglerfish(anglerfish_entity);
		break;

	case COLOUR_STATE::RED:
		redAnglerfish(anglerfish_entity);
		break;

	default: // COLOUR_STATE::NEUTRAL or an undefined state
		neutralAnglerfish(anglerfish_entity);
		break;
	}
}

// For the Anglerfish, the neutral state is just the initial state
void CreatureBehaviourSystem::neutralAnglerfish(Entity& anglerfish_entity)
{

}

void CreatureBehaviourSystem::blueAnglerfish(Entity& anglerfish_entity)
{
	// Lights radius around it in blue, pathfinds to player
}

void CreatureBehaviourSystem::redAnglerfish(Entity& anglerfish_entity)
{
	// lights radius around it in red, pathfinds to player
}