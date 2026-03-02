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
				Decorator(builder, creatureBehaviourFunctions.isInRedState,
					Selector(builder,
						Decorator(builder, creatureBehaviourFunctions.isDizzy,
							Sequence(builder, 
								Action(builder, creatureBehaviourFunctions.beDizzy),
								Selector(builder, 
									Decorator(builder, creatureBehaviourFunctions.isCooldownComplete,
										Sequence(builder,
											Action(builder, creatureBehaviourFunctions.removeCooldown),
											Action(builder, creatureBehaviourFunctions.removeDizzy),
											Action(builder, creatureBehaviourFunctions.makeNeutral)
										)
									),
									Action(builder, creatureBehaviourFunctions.returnSuccess)
								)
							)
						),
						Decorator(builder, creatureBehaviourFunctions.isCharging,
							Selector(builder,
								Decorator(builder, creatureBehaviourFunctions.hasCreatureCollided,
									Sequence(builder, 
										Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
										Action(builder, creatureBehaviourFunctions.removeCollision),
										Action(builder, creatureBehaviourFunctions.makeDizzy),
										Action(builder, creatureBehaviourFunctions.applyCooldown),
										Action(builder, creatureBehaviourFunctions.removeCharging)
									)
								)
							)
						),
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.stopCreatureMovement),
							Action(builder, creatureBehaviourFunctions.applyCooldown),
							Action(builder, creatureBehaviourFunctions.aimAtTarget),
							Selector(builder,
								Decorator(builder, creatureBehaviourFunctions.isCooldownComplete,
									Sequence(builder, 
										Action(builder, creatureBehaviourFunctions.removeCooldown),
										Action(builder, creatureBehaviourFunctions.applyCharging),
										Action(builder, creatureBehaviourFunctions.sawfishChargeAtTarget)
									)
								)
							)
						)
					)
				),
				Selector(builder,
					Decorator(builder, creatureBehaviourFunctions.isInBlueState,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.aStarFollowTarget)
						)
					),
					Decorator(builder, creatureBehaviourFunctions.isInNeutralState,
						Sequence(builder,
							Action(builder, creatureBehaviourFunctions.returnSuccess)
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
				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinCreatureColourRadius,
					Selector(builder,
						Decorator(builder, creatureBehaviourFunctions.isInRedState,
							Action(builder, creatureBehaviourFunctions.redArms)),
						Decorator(builder, creatureBehaviourFunctions.isInBlueState,
							Action(builder, creatureBehaviourFunctions.blueArms)),
						Action(builder, creatureBehaviourFunctions.neutralArms))
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinPlayerColourRadius,
					Selector(builder,
						Decorator(builder, creatureBehaviourFunctions.isInRedState,
							Action(builder, creatureBehaviourFunctions.redArms)),
						Decorator(builder, creatureBehaviourFunctions.isInBlueState,
							Action(builder, creatureBehaviourFunctions.blueArms)),
						Action(builder, creatureBehaviourFunctions.neutralArms))
				),

				Decorator(builder, creatureBehaviourFunctions.isCreatureWithinInteractableColourRadius,
					Selector(builder,
						Decorator(builder, creatureBehaviourFunctions.isInRedState,
							Action(builder, creatureBehaviourFunctions.redArms)),
						Decorator(builder, creatureBehaviourFunctions.isInBlueState,
							Action(builder, creatureBehaviourFunctions.blueArms)),
						Action(builder, creatureBehaviourFunctions.neutralArms))
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
