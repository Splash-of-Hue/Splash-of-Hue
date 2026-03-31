#pragma once
#include <cassert>
#include <ecs/entity.hpp>
#include <type_traits>
#include <vector>

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
	SEQUENCE = SELECTOR + 1, // like A && B && C
	INVERTER = SEQUENCE + 1 // !A
};

struct Node
{
	NODETYPE nodeType;
	std::vector<int> children_indicies;
	BEHAVIOUR_TREE_STATUS(*node_function)(Entity& entity, float elapsed_ms);

	Node(NODETYPE nodeType, std::vector<int> children_indicies = {}, BEHAVIOUR_TREE_STATUS(*node_function)(Entity& entity, float elapsed_ms) = nullptr)
		: nodeType(nodeType), children_indicies(children_indicies), node_function(node_function)
	{}
};

struct BehaviourTree
{
	std::vector<Node> nodes;
	int root_index = 0;
};

struct BehaviourTreeBuilder
{
	BehaviourTree tree;

	int makeNode(NODETYPE nodeType, std::vector<int> children_indicies = {}, BEHAVIOUR_TREE_STATUS(*node_function)(Entity& entity, float elapsed_ms) = nullptr)
	{
		int node_index = tree.nodes.size();
		tree.nodes.emplace_back(Node{ nodeType, std::move(children_indicies), node_function });
		return node_index;
	}

	BehaviourTree build(int root_index)
	{
		tree.root_index = root_index;
		return std::move(tree);
	}
};

inline int Root(BehaviourTreeBuilder& builder, int child_index)
{
	return builder.makeNode(NODETYPE::ROOT, { child_index });
}

inline int Action(BehaviourTreeBuilder& builder, BEHAVIOUR_TREE_STATUS(*node_function)(Entity& entity, float elapsed_ms))
{
	return builder.makeNode(NODETYPE::ACTION, {}, node_function);
}

inline int Decorator(BehaviourTreeBuilder& builder, BEHAVIOUR_TREE_STATUS(*node_function)(Entity& entity, float elapsed_ms), int child_index)
{
	return builder.makeNode(NODETYPE::DECORATOR, { child_index }, node_function);
}

inline int Invert(BehaviourTreeBuilder& builder, BEHAVIOUR_TREE_STATUS(*node_function)(Entity& entity, float elapsed_ms), int child_index)
{
	return builder.makeNode(NODETYPE::INVERTER, { child_index }, node_function);
}

// See Variadic functions: https://stackoverflow.com/questions/1657883/variable-number-of-arguments-in-c
template <typename... Children_indicies>
inline int Selector(BehaviourTreeBuilder& builder, Children_indicies... children_indicies)
{
	assert((std::is_convertible_v<Children_indicies, int> && ...) && "Child indicies must be passed in as ints");
	return builder.makeNode(NODETYPE::SELECTOR, std::vector<int>{ children_indicies... });
}

// See Variadic functions: https://stackoverflow.com/questions/1657883/variable-number-of-arguments-in-c
template <typename... Children_indicies>
inline int Sequence(BehaviourTreeBuilder& builder, Children_indicies... children_indicies...)
{
	assert((std::is_convertible_v<Children_indicies, int> && ...) && "Child indicies must be passed in as ints");
	return builder.makeNode(NODETYPE::SEQUENCE, std::vector<int>{ children_indicies... });
}
