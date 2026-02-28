#pragma once

#include "common.hpp"
#include "creature_system/creature_behaviour_tree_builder.hpp"

#include <iostream>

// TODO: https://trello.com/c/hmk5nWAW/121-task-score-task-template

/*

When creating new behaviour trees, here is some important information to keep in mind:
- Refer to the CPSC427-11-AI-introduction slides (https://canvas.ubc.ca/courses/176823/files/44668926?module_item_id=9045525),
particularly page 45, for more information on the nodes and behaviour trees. 
- The order of the nodes always matters! If you want a certain behaviour to 
have higher priority than another behaviour, make sure to put it before the 
other behaviour in the list of children for that node.
- Decorator nodes can only have 1 child in this implementation.

*/

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
		//if in red state, cannot change states until it crashed into a wall, so this is selector, so we can loop through red state
		Selector(builder,
			Decorator(builder, isInRedState,
				Sequence(builder,
					Action(builder, chargeAtTarget)/*,
					Action(builder, attackTarget), 
					Action(builder, dizzy), 
					Action(builder, turnNeutral)*/
				)
			),
			Sequence(builder, 
				//Do Blue/Neutral Action
				Selector(builder, 
					Decorator(builder, isInBlueState,
						//ignore this makeBlue, it's just so there's an action function here and doesnt give me an error
						Action(builder, makeBlue)/*,
						Action(builder, follow_player)*/
					),
					Decorator(builder, isInNeutralState, 
						//ignore this makeNeutral, it's just so there's an action function here and doesnt give me an error
						Action(builder, makeNeutral)/*,
						Action(builder, wanderAround)*/
					)
				), 
				//Change colour to whatever colour you're in
				Selector(builder, 
					Decorator(builder, isCreatureWithinPlayerColourRadius, 
						Sequence(builder, 
							Selector(builder,
								Decorator(builder, isCreatureInBlueRadius,
									Action(builder, makeBlue)
								),
								Decorator(builder, isCreatureInRedRadius,
								  /*Sequence(builder, 
								        Action(builder, targetCreature),*/
								        Action(builder, makeRed)
								  /*)*/
								),
								Action(builder, makeNeutral)
							),
							Action(builder, stopColliding)
						)
					)
				)
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
