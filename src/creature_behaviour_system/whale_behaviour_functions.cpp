// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_functions.hpp"

void updateWhale(Entity& whale_entity)
{
	if (!registry.colourStates.has(whale_entity))
	{
		ColourState& whaleColourState = registry.colourStates.emplace(whale_entity);
		whaleColourState.curr_colour_state = COLOUR_STATE::NEUTRAL;
	}

	ColourState& whaleColourState = registry.colourStates.get(whale_entity);

	switch (whaleColourState.curr_colour_state)
	{
	case COLOUR_STATE::BLUE:
		blueWhale(whale_entity);
		break;

	case COLOUR_STATE::RED:
		redWhale(whale_entity);
		break;

	default: // COLOUR_STATE::NEUTRAL or an undefined state
		neutralWhale(whale_entity);
		break;
	}
}

void neutralWhale(Entity& whale_entity)
{

}

void blueWhale(Entity& whale_entity)
{
	// cries to fill room above
}

void redWhale(Entity& whale_entity)
{
	// cries to fill room below
}
