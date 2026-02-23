#pragma once

#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_tree.hpp"



void CreatureBehaviourTree::update(float elapsed_ms)
{
	for (Entity& entity : registry.creatureBehaviours.entities)
	{
		CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.get(entity);
		tickNode(entity, creatureBehaviour);
	}
}


BEHAVIOUR_TREE_STATUS CreatureBehaviourTree::tickNode(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index)
{
	Node currentNode = tree.nodes[node_index];

	switch (currentNode.nodeType)
	{
		case NODETYPE::ROOT:
			return tickNode(creature, creatureBehaviour, tree, currentNode.children_indicies[0]);
		case NODETYPE::SEQUENCE:
			return tickSequence(creature, creatureBehaviour, tree, node_index);
		case NODETYPE::SELECTOR:
			return tickSelector(creature, creatureBehaviour, tree, node_index);
		case NODETYPE::DECORATOR:
			return currentNode.node_function(creature);
		case NODETYPE::ACTION:
			return currentNode.node_function(creature);
		default:
			return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
}

int CreatureBehaviourTree::findCurrentChildIndex(CreatureBehaviour& creatureBehaviour, int node_index)
{
	auto it = creatureBehaviour.currentChildNode.find(node_index);
	if (it != creatureBehaviour.currentChildNode.end())
		return it->second;

	return 0;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourTree::tickSequence(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index)
{
	Node currentNode = tree.nodes[node_index];
	int start_child_index = findCurrentChildIndex();

}

BEHAVIOUR_TREE_STATUS CreatureBehaviourTree::tickSelector(Entity& creature, CreatureBehaviour& creatureBehaviour, BehaviourTree& tree, int node_index)
{
	Node currentNode = tree.nodes[node_index];
	int start_child_index = findCurrentChildIndex();

}

/*
BEHAVIOUR_TREE_STATUS CreatureBehaviourTree::tickLeaf(Entity& creature, CreatureBehaviour creatureBehaviour, BehaviourTree& tree, int node_index)
{

} */