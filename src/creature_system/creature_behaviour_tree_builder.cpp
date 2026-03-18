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
	BehaviourTreeBuilder builder;
	int root_index =
		Root(builder,
			Selector(builder,
				Decorator(builder, creatureBehaviourFunctions.isSliding,
					Selector(builder,
						Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.makeNeutral),
								Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
								Action(builder, creatureBehaviourFunctions.removeCollision)
							)
						),
						Action(builder, creatureBehaviourFunctions.returnSuccess)
					)
				),
				
				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinCreatureRedColourRadius,
					Selector(builder,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.makeRed),
							Action(builder, creatureBehaviourFunctions.slideBackwards)
						),
						Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.makeNeutral),
								Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
								Action(builder, creatureBehaviourFunctions.removeCollision)
							)
						)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinCreatureBlueColourRadius,
					Selector(builder,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.makeBlue),
							Action(builder, creatureBehaviourFunctions.slideForwards)
						),
						Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.makeNeutral),
								Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
								Action(builder, creatureBehaviourFunctions.removeCollision)
							)
						)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerRedColourRadius,
					Selector(builder,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.makeRed),
							Action(builder, creatureBehaviourFunctions.slideBackwards)
						),
						Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.makeNeutral),
								Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
								Action(builder, creatureBehaviourFunctions.removeCollision)
							)
						)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerBlueColourRadius,
					Selector(builder,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.makeBlue),
							Action(builder, creatureBehaviourFunctions.slideForwards)
						),
						Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.makeNeutral),
								Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
								Action(builder, creatureBehaviourFunctions.removeCollision)
							)
						)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinInteractableRedColourRadius,
					Selector(builder,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.makeRed),
							Action(builder, creatureBehaviourFunctions.slideBackwards)
						),
						Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.makeNeutral),
								Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
								Action(builder, creatureBehaviourFunctions.removeCollision)
							)
						)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinInteractableBlueColourRadius,
					Selector(builder,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.makeBlue),
							Action(builder, creatureBehaviourFunctions.slideForwards)
						),
						Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.makeNeutral),
								Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
								Action(builder, creatureBehaviourFunctions.removeCollision)
							)
						)
					)
				)
			)
		);

	return builder.build(root_index);
}

BehaviourTree CreatureBehaviourTreeBuilder::buildMinnowTree()
{
	BehaviourTreeBuilder builder;
	int root_index =
		Root(builder,
			Action(builder, creatureBehaviourFunctions.minnowTest));

	return builder.build(root_index);
}

BehaviourTree CreatureBehaviourTreeBuilder::buildSawfishTree()
{
	BehaviourTreeBuilder builder;
	int root_index =
		Root(builder,
			Selector(builder,
				Decorator(builder, creatureBehaviourFunctions.isInRedState,
					Selector(builder,
						Decorator(builder, creatureBehaviourFunctions.isDizzy,
							Selector(builder,
								Decorator(builder, creatureBehaviourFunctions.isCooldownComplete,
									Sequence(builder,
										Action(builder, creatureBehaviourFunctions.removeCooldown),
										Action(builder, creatureBehaviourFunctions.removeDizzy),
										Action(builder, creatureBehaviourFunctions.makeNeutral)
									)
								),
								Action(builder, creatureBehaviourFunctions.beDizzy)
							)
						),
						Decorator(builder, creatureBehaviourFunctions.isCharging,
							Selector(builder, 
								Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
									Sequence(builder, 
										Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
										Action(builder, creatureBehaviourFunctions.removeCollision),
										Action(builder, creatureBehaviourFunctions.removeCharging),
										Action(builder, creatureBehaviourFunctions.applyDizzy),
										Action(builder, creatureBehaviourFunctions.applyCooldown)
									)
								),
								Action(builder, creatureBehaviourFunctions.returnSuccess)
							)
						),
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
							Action(builder, creatureBehaviourFunctions.aimAtTarget),
							Selector(builder,
								Decorator(builder, creatureBehaviourFunctions.isCooldownComplete,
									Sequence(builder,
										Action(builder, creatureBehaviourFunctions.removeCooldown),
										Action(builder, creatureBehaviourFunctions.applyCharging),
										Action(builder, creatureBehaviourFunctions.sawfishChargeAtTarget)
									)
								),
								Action(builder, creatureBehaviourFunctions.returnSuccess)
							)
						)
					)
				),
				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerRedColourRadius,
					Sequence(builder,
						Action(builder, creatureBehaviourFunctions.applyCooldown),
						Action(builder, creatureBehaviourFunctions.makeRed)
					)
				),
				Sequence(builder,
					Selector(builder, 
						Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerBlueColourRadius,
							Action(builder, creatureBehaviourFunctions.makeBlue)
						),
						Action(builder, creatureBehaviourFunctions.returnSuccess)
					),

					Selector(builder,
						Decorator(builder, creatureBehaviourFunctions.isInBlueState,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.aimAtTarget),
								Action(builder, creatureBehaviourFunctions.aStarFollowTarget)
							)
						),
						Decorator(builder, creatureBehaviourFunctions.isInNeutralState,
							Sequence(builder,
								Action(builder, creatureBehaviourFunctions.aimAtTarget)
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
	/*
	
		Shrimp prioritizes creature light over player light.
		Unsure what to do about the duplicated code here
	
	*/

	BehaviourTreeBuilder builder;
	int root_index =
		Root(builder,
			Selector(builder,
				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinCreatureRedColourRadius,
					Sequence(builder,
						Action(builder, creatureBehaviourFunctions.makeRed),
						Action(builder, creatureBehaviourFunctions.redArms)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinCreatureBlueColourRadius,
					Sequence(builder,
						Action(builder, creatureBehaviourFunctions.makeBlue),
						Action(builder, creatureBehaviourFunctions.blueArms)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerRedColourRadius,
					Sequence(builder, 
						Action(builder, creatureBehaviourFunctions.makeRed),
						Action(builder, creatureBehaviourFunctions.redArms)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerBlueColourRadius,
					Sequence(builder,
						Action(builder, creatureBehaviourFunctions.makeBlue),
						Action(builder, creatureBehaviourFunctions.blueArms)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinInteractableRedColourRadius,
					Sequence(builder,
						Action(builder, creatureBehaviourFunctions.makeRed),
						Action(builder, creatureBehaviourFunctions.redArms)
					)
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinInteractableBlueColourRadius,
					Sequence(builder,
						Action(builder, creatureBehaviourFunctions.makeBlue),
						Action(builder, creatureBehaviourFunctions.blueArms)
					)
				),

				Sequence(builder,
					Action(builder, creatureBehaviourFunctions.makeNeutral),
					Action(builder, creatureBehaviourFunctions.neutralArms))

				)
		);

	return builder.build(root_index);
}

BehaviourTree CreatureBehaviourTreeBuilder::buildWhaleTree()
{
	return BehaviourTree();
}
