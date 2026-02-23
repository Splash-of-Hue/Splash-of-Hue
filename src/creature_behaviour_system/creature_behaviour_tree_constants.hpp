#pragma once

enum class BEHAVIOUR_TREE_STATUS
{
	FAILURE = 0,
	SUCCESS = FAILURE + 1,
	RUNNING = SUCCESS + 1
};

enum class NODETYPE
{
	ROOT = 0,
	ACTION = ROOT + 1,
	DECORATOR = ACTION + 1, // A question with 1 child node
	SELECTOR = DECORATOR + 1, // like A || B || C
	SEQUENCE = SELECTOR + 1 // like A && B && C
};

struct Node
{
	NODETYPE nodeType;
	std::vector<int> children_indicies;
	BEHAVIOUR_TREE_STATUS(*node_function)(Entity& entity, float elapsed_ms);
};

struct BehaviourTree
{
	std::vector<Node> nodes;
	int root_index;
};
