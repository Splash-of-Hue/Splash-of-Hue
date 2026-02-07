#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

// System responsible for handling creature behaviours. 
class CreatureBehaviourSystem
{

public:
	void step(float elapsed_ms)
	{
		for (Creature& creature_component : registry.creatures.components)
		{
			switch (creature_component.creatureType)
			{
			case ANGLERFISH:
				updateAnglerfish();
				break;
			case CRAB:
				updateCrab();
				break;
			case MINNOWS:
				updateMinnows();
				break;
			case SAWFISH:
				updateSawfish(elapsed_ms);
				break;
			case SHRIMP:
				updateShrimp();
				break;
			case WHALE:
				updateWhale();
				break;
			default: // UNDEFINED
				updateCreature();
				break;
			}
		}
	}

private:
	void updateAnglerfish();
	void updateCrab();
	void updateCreature();
	void updateMinnows();
	void updateSawfish(float elapsed_ms);
	void updateShrimp();
	void updateWhale();

	// Coloured behaviours for each creature.
	void neutralAnglerfish();
	void blueAnglerfish();
	void redAnglerfish();

	void neutralCrab();
	void blueCrab();
	void redCrab();

	void neutralCreature();
	void blueCreature();
	void redCreature();

	void neutralMinnows();
	void blueMinnows();
	void redMinnows();

	void neutralSawfish();
	void blueSawfish();
	void redSawfish();

	void neutralShrimp();
	void blueShrimp();
	void redShrimp();

	void neutralWhale();
	void blueWhale();
	void redWhale();

	// Pathfinder
	void follow_player(float elapsed_ms, Entity& follower);
};



// step function
// template class that has a blue behaviour, red behaviour, neutral behaviour 
// in hpp:
// map that maps behaviour types to behaviour systems 
// multiple cpp files:
//	behavour shrimp system
// 
// hpp file has all of the shrimp functions, whale functions
// 
// similar to loader system or how render init is done, you can have
// files be their own thing 
// 
// if i only want to look at how the shrimp behaves, the hpp has all of the funcitons,
// but the cpp you have multiple 
// 
// 
// for extendability: making a changeable behaviour system T (generic/template) 
// then entity would have a behaviour type (shrimp) and a colour state (blue) 
// behaviour system ~ everything that has a behaviour, everything that has a type,
// a state, here is the system, each function 
// 
// 
// 
// 
// ai system
// shrimp, want to update behaviour, in shrimp update
// you know you do have a colour state, so assert get colour state