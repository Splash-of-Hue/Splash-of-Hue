#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

// System responsible for handling creature behaviours. 
/*
*
	Every function created to be called by the behaviour tree (action nodes and decorator nodes) should be declared here
	and defined in creature_behaviour_functions.cpp

	WHEN CREATING NEW FUNCTIONS HERE:
	Currently, the definition for a function in a Node REQUIRES that each function can only (and must be) declared
	with parameters Entity& creature, float elapsed_ms, and return a BEHAVIOUR_TREE_STATUS. Keep this in mind when
	designing your functions, and try to get any necessary information from components attached to the creature
	entity instead of passing them in.

*/
class CreatureBehaviourFunctions
{
public:
	// Pathfinder, TEMPORARY to be discontinued once A* pathfinding is implemented. (Qj457xrS)
	bool follow_player(float elapsed_ms, Entity& follower);

	// ===== Action Node Functions =====
	static BEHAVIOUR_TREE_STATUS aStarFollowTarget(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS chargeAtTarget(Entity& charger, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS stopCreatureMovement(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS makeRed(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS makeBlue(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS makeNeutral(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS stopColliding(Entity& creature, float elapsed_ms);

	// Shrimp Behaviour Actions
	static BEHAVIOUR_TREE_STATUS neutralArms(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS blueArms(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS redArms(Entity& follower, float elapsed_ms);

	// ===== Decorator Node Functions =====
	static BEHAVIOUR_TREE_STATUS isInBlueState(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isInRedState(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isInNeutralState(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCooldownComplete(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isTimerComplete(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinAnyColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinPlayerColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinCreatureColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinInteractableColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureInBlueRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureInRedRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS hasCreatureCollided(Entity& creature, float elapsed_ms);

private:
	// ===== NODE HELPERS =====
	// ===== Action Helper Functions =====
	// General Helpers
	static BEHAVIOUR_TREE_STATUS makeColour(Entity& creature, COLOUR_STATE colour_state);
	
	// ===== Decorator Helper Functions =====
	// General Helpers
	template <typename Component>
	static BEHAVIOUR_TREE_STATUS checkIfInComponentRadius(Entity& creature, Component& component_type);
	static BEHAVIOUR_TREE_STATUS checkIfInColourRadius(Entity& creature, COLOUR_STATE colour_state);


	// Shrimp helpers
	static BEHAVIOUR_TREE_STATUS disableCollidableComponent(std::vector<Entity>& entityList);
	static BEHAVIOUR_TREE_STATUS enableCollidableComponent(std::vector<Entity>& entityList);
	static BEHAVIOUR_TREE_STATUS setShrimpArms(Entity& creature, SHRIMP_ARM_COLOUR shrimp_arm_colour);

};
extern CreatureBehaviourFunctions creatureBehaviourFunctions;
