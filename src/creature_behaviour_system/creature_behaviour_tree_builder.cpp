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
	BehaviourTreeBuilder builder;
	int root_index =
		Root(builder,
			Selector(builder,
				Decorator(builder, isInBlueState,
					Selector(builder,
						Action(builder, aStarFollowTarget),
						Action(builder, stopCreatureMovement)
					)
				),
				Decorator(builder, isInRedState,
					Action(builder, chargeAtTarget)
				),
				Decorator(builder, isInNeutralState,
					Action(builder, stopCreatureMovement)
				)
			)
		);

	return builder.build(root_index);
}

BehaviourTree CreatureBehaviourTreeBuilder::buildShrimpTree()
{
	return BehaviourTree();
}

BehaviourTree CreatureBehaviourTreeBuilder::buildWhaleTree()
{
	return BehaviourTree();
}
