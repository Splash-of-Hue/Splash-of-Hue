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
			Selector(builder,
				Decorator(builder, creatureBehaviourFunctions.isInBlueState,
					Selector(builder,
						Action(builder, creatureBehaviourFunctions.aStarFollowTarget),
						Action(builder, creatureBehaviourFunctions.stopCreatureMovement)
					)
				),
				Decorator(builder, creatureBehaviourFunctions.isInRedState,
					Action(builder, creatureBehaviourFunctions.chargeAtTarget)
				),
				Decorator(builder, creatureBehaviourFunctions.isInNeutralState,
					Action(builder, creatureBehaviourFunctions.stopCreatureMovement)
				)
			)
		);

	return builder.build(root_index);
}

BehaviourTree CreatureBehaviourTreeBuilder::buildShrimpTree()
{
	/*
	
		Shrimp prioritizes creature light over player light.
		Unsure what to do about the duplicated code here
	
	*/
	BehaviourTreeBuilder builder;
	int root_index =
		Root(builder,
			Selector(builder,
				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinCreatureColourRadius,
					Sequence(builder,
						Selector(builder,
							Decorator(builder, creatureBehaviourFunctions.isCreatureInBlueRadius,
								Sequence(builder,
									Action(builder, creatureBehaviourFunctions.blueArms)
								)
							),
							Decorator(builder, creatureBehaviourFunctions.isCreatureInRedRadius,
								Sequence(builder,
									Action(builder, creatureBehaviourFunctions.redArms)
								)
							)
						),
						Action(builder, creatureBehaviourFunctions.stopColliding)
					)
				), 

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerColourRadius,
					Sequence(builder,
						Selector(builder,
							Decorator(builder, creatureBehaviourFunctions.isCreatureInBlueRadius,
								Sequence(builder,
									Action(builder, creatureBehaviourFunctions.blueArms)
								)
							),
							Decorator(builder, creatureBehaviourFunctions.isCreatureInRedRadius,
								Sequence(builder,
									Action(builder, creatureBehaviourFunctions.redArms)
								)
							)
						),
						Action(builder, creatureBehaviourFunctions.stopColliding)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinInteractableColourRadius,
					Sequence(builder,
						Selector(builder,
							Decorator(builder, creatureBehaviourFunctions.isCreatureInBlueRadius,
								Sequence(builder,
									Action(builder, creatureBehaviourFunctions.blueArms)
								)
							),
							Decorator(builder, creatureBehaviourFunctions.isCreatureInRedRadius,
								Sequence(builder,
									Action(builder, creatureBehaviourFunctions.redArms)
								)
							)
						),
						Action(builder, creatureBehaviourFunctions.stopColliding)
					)
				),
				Action(builder, creatureBehaviourFunctions.neutralArms)
			)
		

		);

	return builder.build(root_index);
}

BehaviourTree CreatureBehaviourTreeBuilder::buildWhaleTree()
{
	return BehaviourTree();
}
