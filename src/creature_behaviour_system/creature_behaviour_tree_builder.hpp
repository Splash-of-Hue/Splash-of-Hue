#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

#include "creature_behaviour_functions.hpp"

/* This is where all of the behaviour trees for each creature type can be built! */

class CreatureBehaviourTreeBuilder
{

public:
	BehaviourTree buildCreatureTreeBasedOnType(CREATURETYPE creatureType)
	{
		switch (creatureType)
		{
		case ANGLERFISH:
			return buildAnglerfishTree();
		case CRAB:
			return buildCrabTree();
		case MINNOWS:
			return buildMinnowTree();
		case SAWFISH:
			return buildSawfishTree();
		case SHRIMP:
			return buildShrimpTree();
		case WHALE:
			return buildWhaleTree();
		default:
			assert(false && "Invalid creature type passed into buildCreatureTreeBasedOnType");
			return BehaviourTree();
		}
	}

	BehaviourTree buildAnglerfishTree();
	BehaviourTree buildCrabTree();
	BehaviourTree buildMinnowTree();
	BehaviourTree buildSawfishTree();
	BehaviourTree buildShrimpTree();
	BehaviourTree buildWhaleTree();
};
