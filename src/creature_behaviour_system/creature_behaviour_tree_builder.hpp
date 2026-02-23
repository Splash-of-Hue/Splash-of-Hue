#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

/* This is where all of the behaviour trees for each creature type can be built! */

class CreatureBehaviourTreeBuilder
{

public:
	BehaviourTree buildAnglerfishTree();
	BehaviourTree buildCrabTree();
	BehaviourTree buildMinnowTree();
	BehaviourTree buildSawfishTree();
	BehaviourTree buildShrimpTree();
	BehaviourTree buildWhaleTree();
};