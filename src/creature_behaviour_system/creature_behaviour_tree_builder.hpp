#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

#include "src/creature_behaviour_system/creature_behaviour_tree_constants.hpp"

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