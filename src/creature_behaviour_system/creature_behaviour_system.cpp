#pragma once

#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"
#include <cassert>
#include <sstream>
#include <iostream>

void CreatureBehaviourSystem::step(float elapsed_ms)
{
	for (Entity& entity : registry.creatureBehaviours.entities)
	{
		CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.get(entity);
		int root_index = creatureBehaviour.behaviour_tree.root_index;
		tickNode(entity, creatureBehaviour, root_index, elapsed_ms);
	}
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourSystem::tickNode(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms)
{
	BehaviourTree& tree = creatureBehaviour.behaviour_tree;
	Node& currentNode = tree.nodes[node_index];

	switch (currentNode.nodeType)
	{
		case NODETYPE::ROOT:
			assert(!currentNode.children_indicies.empty() && "Root node has no children!");
			return tickNode(creature, creatureBehaviour, currentNode.children_indicies[0], elapsed_ms);

		case NODETYPE::SEQUENCE:
			return tickSequence(creature, creatureBehaviour, node_index, elapsed_ms);

		case NODETYPE::SELECTOR:
			return tickSelector(creature, creatureBehaviour, node_index, elapsed_ms);

		case NODETYPE::DECORATOR:
			return tickDecorator(creature, creatureBehaviour, node_index, elapsed_ms);

		case NODETYPE::ACTION:
			return currentNode.node_function(creature, elapsed_ms);

		default:
			return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
}

int CreatureBehaviourSystem::findCurrentChildIndex(CreatureBehaviour& creatureBehaviour, int node_index)
{
	auto it = creatureBehaviour.currentChildNode.find(node_index);
	if (it != creatureBehaviour.currentChildNode.end())
		return it->second;

	return 0;
}

// Sequence nodes are like AND operations, so all children must succeed
BEHAVIOUR_TREE_STATUS CreatureBehaviourSystem::tickSequence(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms)
{
	BehaviourTree& tree = creatureBehaviour.behaviour_tree;
	Node& currentNode = tree.nodes[node_index];
	int start_child_index = findCurrentChildIndex(creatureBehaviour, node_index);

	for (int i = start_child_index; i < currentNode.children_indicies.size(); i++)
	{
		int child_index = currentNode.children_indicies[i];
		BEHAVIOUR_TREE_STATUS child_status = tickNode(creature, creatureBehaviour, child_index, elapsed_ms);

		if (child_status == BEHAVIOUR_TREE_STATUS::SUCCESS)
		{
			continue;
		}
		else if (child_status == BEHAVIOUR_TREE_STATUS::RUNNING)
		{
			creatureBehaviour.currentChildNode[node_index] = i;
			return BEHAVIOUR_TREE_STATUS::RUNNING;
		}
		else if (child_status == BEHAVIOUR_TREE_STATUS::FAILURE)
		{
			creatureBehaviour.currentChildNode.erase(node_index);
			return BEHAVIOUR_TREE_STATUS::FAILURE;
		}
	}

	// All of the children completed successfully so we can remove the benchmark we set for ourselves (currentChildNode) and return success! :)
	creatureBehaviour.currentChildNode.erase(node_index);
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

// Selector nodes are like OR operations, so only one child needs to succeed
BEHAVIOUR_TREE_STATUS CreatureBehaviourSystem::tickSelector(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms)
{
	BehaviourTree& tree = creatureBehaviour.behaviour_tree;
	Node& currentNode = tree.nodes[node_index];
	int start_child_index = findCurrentChildIndex(creatureBehaviour, node_index);

	for (int i = start_child_index; i < currentNode.children_indicies.size(); i++)
	{
		int child_index = currentNode.children_indicies[i];
		BEHAVIOUR_TREE_STATUS child_status = tickNode(creature, creatureBehaviour, child_index, elapsed_ms);

		if (child_status == BEHAVIOUR_TREE_STATUS::SUCCESS)
		{
			creatureBehaviour.currentChildNode.erase(node_index);
			return BEHAVIOUR_TREE_STATUS::SUCCESS;
		}
		else if (child_status == BEHAVIOUR_TREE_STATUS::RUNNING)
		{
			creatureBehaviour.currentChildNode[node_index] = i;
			return BEHAVIOUR_TREE_STATUS::RUNNING;
		}
		else if (child_status == BEHAVIOUR_TREE_STATUS::FAILURE)
		{
			continue;
		}
	}

	// None of the children successfully completed so we can remove the benchmark we set for ourselves (currentChildNode) and return failure
	creatureBehaviour.currentChildNode.erase(node_index);
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourSystem::tickDecorator(Entity& creature, CreatureBehaviour& creatureBehaviour, int node_index, float elapsed_ms)
{
	BehaviourTree& tree = creatureBehaviour.behaviour_tree;
	Node& currentNode = tree.nodes[node_index];

	assert(currentNode.children_indicies.size() == 1 && "Decorator node should not have less/more than one child!");
	BEHAVIOUR_TREE_STATUS status = currentNode.node_function(creature, elapsed_ms);
	if (status != BEHAVIOUR_TREE_STATUS::SUCCESS)
		return status;

	return tickNode(creature, creatureBehaviour, currentNode.children_indicies[0], elapsed_ms);
}
