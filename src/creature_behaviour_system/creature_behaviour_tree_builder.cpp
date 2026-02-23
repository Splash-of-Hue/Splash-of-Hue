#pragma once

#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_tree_builder.hpp"

#include <iostream>

// TODO: https://trello.com/c/hmk5nWAW/121-task-score-task-template

BehaviourTree CreatureBehaviourTreeBuilder::buildAnglerfishTree()
{
	return BehaviourTree();
}

BehaviourTree CreatureBehaviourTreeBuilder::buildCrabTree()
{
	return BehaviourTree();
}

BehaviourTree CreatureBehaviourTreeBuilder::buildMinnowTree()
{
	return BehaviourTree();
}

BehaviourTree CreatureBehaviourTreeBuilder::buildSawfishTree()
{
	// DEBUGGING std::cout << "build sawfish tree" << std::endl;
	
	BehaviourTree tree;
	// Root
	tree.nodes.push_back({ NODETYPE::ROOT, {7}, nullptr }); // index 0
	// Actions
	tree.nodes.push_back({ NODETYPE::ACTION, {}, aStarFollowTarget}); // index 1
	tree.nodes.push_back({ NODETYPE::ACTION, {}, chargeAtTarget}); // index 2
	tree.nodes.push_back({ NODETYPE::ACTION, {}, stopCreatureMovement }); // index 3

	// Decorators
	tree.nodes.push_back({ NODETYPE::DECORATOR, {8}, isInBlueState }); // index 4
	tree.nodes.push_back({ NODETYPE::DECORATOR, {2}, isInRedState }); // index 5
	tree.nodes.push_back({ NODETYPE::DECORATOR, {3}, isInNeutralState }); // index 6

	// Selectors
	tree.nodes.push_back({ NODETYPE::SELECTOR, {4, 5, 6}, nullptr }); // index 7
	tree.nodes.push_back({ NODETYPE::SELECTOR, {1, 3}, nullptr }); // index 8

	return tree;
}

BehaviourTree CreatureBehaviourTreeBuilder::buildShrimpTree()
{
	return BehaviourTree();
}

BehaviourTree CreatureBehaviourTreeBuilder::buildWhaleTree()
{
	return BehaviourTree();
}