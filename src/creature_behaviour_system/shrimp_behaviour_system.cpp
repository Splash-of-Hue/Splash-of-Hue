// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

void CreatureBehaviourSystem::updateShrimp(Entity& shrimp_entity)
{
	if (!registry.colourStates.has(shrimp_entity))
	{
		ColourState& shrimpColourState = registry.colourStates.emplace(shrimp_entity);
		shrimpColourState.curr_colour_state = COLOUR_STATE::NEUTRAL;
	}

	ColourState& shrimpColourState = registry.colourStates.get(shrimp_entity);

	switch (shrimpColourState.curr_colour_state)
	{
	case COLOUR_STATE::BLUE:
		blueShrimp(shrimp_entity);
		break;

	case COLOUR_STATE::RED:
		redShrimp(shrimp_entity);
		break;

	default: // COLOUR_STATE::NEUTRAL or an undefined state
		neutralShrimp(shrimp_entity);
		break;
	}
}

// For the shrimp, the neutral state is the initial state
void CreatureBehaviourSystem::neutralShrimp(Entity& shrimp_entity)
{

}

void CreatureBehaviourSystem::blueShrimp(Entity& shrimp_entity)
{
	// punches Up & Right
	// switching depends on if it's touched again w blue

	// Up: should create an arm that's not collidable above the shrimp 
}

void CreatureBehaviourSystem::redShrimp(Entity& shrimp_entity)
{
	// punches left and down
	// switching depends on if its touched again w red
}