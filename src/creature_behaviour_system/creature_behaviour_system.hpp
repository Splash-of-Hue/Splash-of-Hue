#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

#include "creature_behaviour_system/creature_behaviour_functions.hpp"
#include "creature_behaviour_system/creature_behaviour_tree_constants.hpp"

class CreatureBehaviourSystem
{

public:
	void update(float elapsed_ms);

private:
	int findCurrentChildIndex(CreatureBehaviour& creatureBehaviour, int node_index);
	BEHAVIOUR_TREE_STATUS tickNode(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index, float elapsed_ms);
	BEHAVIOUR_TREE_STATUS tickSequence(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index, float elapsed_ms);
	BEHAVIOUR_TREE_STATUS tickSelector(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index, float elapsed_ms);
	BEHAVIOUR_TREE_STATUS tickDecorator(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index, float elapsed_ms);
};
