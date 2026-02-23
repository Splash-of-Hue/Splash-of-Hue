#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

#include "src/creature_behaviour_system/creature_behaviour_tree_constants.hpp"

class CreatureBehaviourTree
{

public:
	void update(float elapsed_ms);

private:
	int findCurrentChildIndex(CreatureBehaviour& creatureBehaviour, int node_index);
	BEHAVIOUR_TREE_STATUS tickNode(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index);
	BEHAVIOUR_TREE_STATUS tickSequence(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index);
	BEHAVIOUR_TREE_STATUS tickSelector(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index);
	//BEHAVIOUR_TREE_STATUS tickDecorator(Entity& creature, CreatureBehaviour creatureBehaviour, BehaviourTree& tree, int node_index);
	//BEHAVIOUR_TREE_STATUS tickLeaf(Entity& creature, CreatureBehaviour creatureBehaviour, BehaviourTree& tree, int node_index);
}