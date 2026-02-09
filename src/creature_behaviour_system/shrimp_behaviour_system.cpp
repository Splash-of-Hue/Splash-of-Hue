// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

// Please see trello task: https://trello.com/c/CRvulzMB
void CreatureBehaviourSystem::updateShrimp(Entity& shrimp_entity)
{
	if (!registry.colourStates.has(shrimp_entity))
	{
		// Initialize shrimp
		ColourState& shrimpColourState = registry.colourStates.emplace(shrimp_entity);
		shrimpColourState.curr_colour_state = COLOUR_STATE::NEUTRAL;
		neutralShrimp(shrimp_entity);
		return;
	}

	ColourState& shrimpColourState = registry.colourStates.get(shrimp_entity);

	if (shrimpColourState.prev_colour_state == shrimpColourState.curr_colour_state)
	{
		// No need to change anything
		return;
	}

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
	// create an arm that's not collidable above the shrimp 


}

void CreatureBehaviourSystem::redShrimp(Entity& shrimp_entity)
{
	// create an arm that's not collidable below the shrimp
}
