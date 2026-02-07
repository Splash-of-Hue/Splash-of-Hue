// internal
#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

// Sophia note:
/*
	It might make more sense for just the leader minnow to have the creature component
	and then for all of the follower minnows to have a "Minnows" component which points
	to the leader minnow entity

	this way we don't have a bajillion creature components for all of the minnows
*/
void CreatureBehaviourSystem::updateMinnows(float elapsed_ms, Entity& minnow_entity)
{
	if (!registry.colourStates.has(minnow_entity))
	{
		ColourState& minnowColourState = registry.colourStates.emplace(minnow_entity);
		minnowColourState.curr_colour_state = COLOUR_STATE::NEUTRAL;
	}

	ColourState& minnowColourState = registry.colourStates.get(minnow_entity);

	switch (minnowColourState.curr_colour_state)
	{
	case COLOUR_STATE::BLUE:
		blueMinnows(minnow_entity);
		break;

	case COLOUR_STATE::RED:
		redMinnows(minnow_entity);
		break;

	default: // COLOUR_STATE::NEUTRAL or an undefined state
		neutralMinnows(minnow_entity);
		break;
	}
}

void CreatureBehaviourSystem::neutralMinnows(Entity& minnow_entity)
{

}

void CreatureBehaviourSystem::blueMinnows(Entity& minnow_entity)
{
	// swarms light source
}

void CreatureBehaviourSystem::redMinnows(Entity& minnow_entity)
{
	// shoots at light source
}