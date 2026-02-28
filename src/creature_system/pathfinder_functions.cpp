// stlib
#include <iostream>
#include <sstream>

#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
#include "creature_system/creature_behaviour_system.hpp"
#include "creature_system/creature_components.hpp"

// todo: https://trello.com/c/Qj457xrS/105-2-creature-behaviour-system-a-pathfinding
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::aStarFollowTarget(Entity& follower, float elapsed_ms)
{
	if (creatureBehaviourFunctions.follow_player(elapsed_ms, follower))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	return BEHAVIOUR_TREE_STATUS::RUNNING;
}

// todo: https://trello.com/c/D5ie5Ml8/120-task-score-task-template
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::chargeAtTarget(Entity& charger, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

// TODO: REMOVE AS PART OF THE ASTAR TASK 
// For M1, no real fancy-schmancy AI pathfinding, just bare bones following 
bool CreatureBehaviourFunctions::follow_player(float elapsed_ms, Entity& follower)
{ 
	assert(registry.pathfinders.has(follower) && "The follower creature isn't registered as a pathfinder");
	Pathfinder& pathfinder_component = registry.pathfinders.get(follower);

	assert(registry.players.entities.size() == 1 && "There's no player for the creature to follow!");
	Entity& player = registry.players.entities[0];

	// TODO: REFACTOR as part of pathfinding tasks in later milestones SO THIS IS GENERAL FOR ANY "TARGET". Whatever entity's ColourComponent triggers it
	assert(registry.positions.has(player) && "Player doesn't have a position");
	assert(registry.positions.has(follower) && "Pathfinder doesn't have a position");
	assert(registry.velocities.has(follower) && "Pathfinder doesn't have a velocity");

	const vec2& player_position = registry.positions.get(player);
	vec2& follower_position = registry.positions.get(follower);
	vec2& follower_velocity = registry.velocities.get(follower);

	// Get the vector to the player
	float dx = player_position.x - follower_position.x;
	float dy = player_position.y - follower_position.y;

	// Calculate and compare squares bec they're cheaper to avoid sqrt
	float distance_squared = dx * dx + dy * dy;
	float minDistToTarget_squared = pathfinder_component.minimumDistanceToTarget * pathfinder_component.minimumDistanceToTarget;

	// If too close to the target, stop
	if (distance_squared <= minDistToTarget_squared)
	{
		follower_velocity = { 0.0f, 0.0f };
		return true;
	}

	// Sqrt only when moving for normalization
	float distance = std::sqrt(distance_squared);
	float norm_x = dx / distance;
	float norm_y = dy / distance;

	// Slow down when approaching the player so there isn't an abrupt stop
	float slow_radius = pathfinder_component.minimumDistanceToTarget * 3.f;
	float max_speed = pathfinder_component.max_speed * elapsed_ms * 0.01;
	if (distance < slow_radius) {
		float t = (distance - pathfinder_component.minimumDistanceToTarget) / (slow_radius - pathfinder_component.minimumDistanceToTarget);
		t = std::clamp(t, 0.f, 1.f);
		max_speed = max_speed * t;
	}  

	follower_velocity = { norm_x * max_speed, norm_y * max_speed };	
	return false;
}
