#pragma once

#include "common.hpp"
#include "creature_system/creature_behaviour_tree_constants.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"
#include "ecs/tiny_ecs.hpp"

class CreatureBehaviourSystem : public SystemInterface
{

public:
	void step(float elapsed_ms);

private:
	int findCurrentChildIndex(CreatureBehaviour& creatureBehaviour, int node_index);
	BEHAVIOUR_TREE_STATUS tickNode(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms);
	BEHAVIOUR_TREE_STATUS tickSequence(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms);
	BEHAVIOUR_TREE_STATUS tickSelector(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms);
	BEHAVIOUR_TREE_STATUS tickDecorator(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms);
	BEHAVIOUR_TREE_STATUS tickInverter(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms);
};
