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
	// Pathing
	static BEHAVIOUR_TREE_STATUS aStarFollowTarget(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS goToNextPathStep(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS requestNewPath(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS requestNewRandomPath(Entity& creature, float elapsed_ms);
	// Colour
	static BEHAVIOUR_TREE_STATUS makeBlue(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS makeRed(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS makeNeutral(Entity& creature, float elapsed_ms);
	// Movement
	static BEHAVIOUR_TREE_STATUS stopCreatureMovement(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS removeCollision(Entity& creature, float elapsed_ms);
	// Miscellaneous
	static BEHAVIOUR_TREE_STATUS applyCooldown(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS removeCooldown(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS returnSuccess(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS returnFailure(Entity& creature, float elapsed_ms);
	// Helper action nodes
	static BEHAVIOUR_TREE_STATUS chargeAtPlayer(Entity& charger, float speed);

	// Shrimp Behaviour Actions
	static BEHAVIOUR_TREE_STATUS neutralArms(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS blueArms(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS redArms(Entity& follower, float elapsed_ms);

	// Sawfish Behaviour Actions
	static BEHAVIOUR_TREE_STATUS applyCharging(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS removeCharging(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS applyDizzy(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS beDizzy(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS removeDizzy(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS aimAtTarget(Entity& follower, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS sawfishChargeAtTarget(Entity& follower, float elapsed_ms);

	// ===== Decorator Node Functions =====
	// Colour
	//todo: https://trello.com/c/SPoplE91/184-1-consolidate-inxradius-components
	static BEHAVIOUR_TREE_STATUS isCreatureWithinAnyColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinPlayerBlueColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinPlayerRedColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinCreatureBlueColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinCreatureRedColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinInteractableBlueColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinInteractableRedColourRadius(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isInBlueState(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isInNeutralState(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isInRedState(Entity& creature, float elapsed_ms);
	// Movement
	static BEHAVIOUR_TREE_STATUS hasCreatureCollided(Entity& creature, float elapsed_ms);
	// Miscellaneous
	static BEHAVIOUR_TREE_STATUS isCooldownComplete(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isTimerComplete(Entity& creature, float elapsed_ms);

	// Sawfish Decorator functions
	static BEHAVIOUR_TREE_STATUS isCharging(Entity& creature, float elapsed_ms);
	static BEHAVIOUR_TREE_STATUS isDizzy(Entity& creature, float elapsed_ms);

private:
	// ===== NODE HELPERS =====
	static BEHAVIOUR_TREE_STATUS makeColour(Entity& creature, COLOUR_STATE colour_state);
	static BEHAVIOUR_TREE_STATUS isCreatureWithinSpecificColourRadius(Entity& creature, bool& inRadius, std::vector<Entity>& radius_list, COLOUR_STATE colour_state);
	// Shrimp helpers
	static BEHAVIOUR_TREE_STATUS disableCollidableComponent(std::vector<Entity>& entityList);
	static BEHAVIOUR_TREE_STATUS enableCollidableComponent(std::vector<Entity>& entityList);
	static BEHAVIOUR_TREE_STATUS setShrimpArms(Entity& creature, SHRIMP_ARM_COLOUR shrimp_arm_colour);

};
extern CreatureBehaviourFunctions creatureBehaviourFunctions;
