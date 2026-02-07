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
		for (Entity& creature : registry.creatures.entities)
		{

			Creature& creature_component = registry.creatures.get(creature);

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
				updateSawfish(elapsed_ms, creature, creature_component);
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
	void updateAnglerfish(float elapsed_ms, Entity& anglerfish_entity);
	void updateCrab(Entity& crab_entity);
	void updateCreature(Entity& creature_entity);
	void updateMinnows(float elapsed_ms, Entity& minnow_entity);
	void updateSawfish(float elapsed_ms, Entity& sawfish_entity, Creature& creature_component);
	void updateShrimp(Entity& shrimp_entity);
	void updateWhale(Entity& whale_entity);

	// Coloured behaviours for each creature.
	void neutralAnglerfish(Entity& anglerfish_entity);
	void blueAnglerfish(Entity& anglerfish_entity);
	void redAnglerfish(Entity& anglerfish_entity);

	void neutralCrab(Entity& crab_entity);
	void blueCrab(Entity& crab_entity);
	void redCrab(Entity& crab_entity);

	void neutralCreature(Entity& creature_entity);
	void blueCreature(Entity& creature_entity);
	void redCreature(Entity& creature_entity);

	void neutralMinnows(Entity& minnow_entity);
	void blueMinnows(Entity& minnow_entity);
	void redMinnows(Entity& minnow_entity);

	void neutralSawfish(Entity& sawfish_entity);
	void blueSawfish(Entity& sawfish_entity);
	void redSawfish(float elapsed_ms, Entity& sawfish_entity);

	void neutralShrimp(Entity& shrimp_entity);
	void blueShrimp(Entity& shrimp_entity);
	void redShrimp(Entity& shrimp_entity);

	void neutralWhale(Entity& whale_entity);
	void blueWhale(Entity& whale_entity);
	void redWhale(Entity& whale_entity);

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