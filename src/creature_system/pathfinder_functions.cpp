// stlib
#include <iostream>
#include <sstream>

#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
#include "creature_system/creature_behaviour_system.hpp"
#include "creature_system/creature_components.hpp"
#include <ai/pathfinding.hpp>

// todo: https://trello.com/c/Qj457xrS/105-2-creature-behaviour-system-a-pathfinding
BEHAVIOUR_TREE_STATUS aStarFollowTarget(Entity& follower, float elapsed_ms)
{
	if (direct_line_next_step(elapsed_ms, follower))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	return BEHAVIOUR_TREE_STATUS::RUNNING;
}

// todo: https://trello.com/c/D5ie5Ml8/120-task-score-task-template
BEHAVIOUR_TREE_STATUS chargeAtTarget(Entity& charger, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS requestNewPath(Entity& pathfinder_entity, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;
	Pathfinder& pathfinder = registry.pathfinders.get(pathfinder_entity);
	pathfinder.is_path_dirty = true;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS goToNextPathStep(Entity& pathfinder_entity, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;

	vec2& position = registry.positions.get(pathfinder_entity);
	Pathfinder& pathfinder = registry.pathfinders.get(pathfinder_entity);

	// If there is no path, return failure as we try to move without any target
	if (pathfinder.path.size() == 0)
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	vec2& next_step = pathfinder.path[0];
	float movement_speed = pathfinder.max_speed * elapsed_ms * ONE_THOUSANDTH;
	float distance_to_next_step = distance(position, next_step);

	if (pathfinder.path.size() == 1)
	{
		// We are at the last step of the path, so don't go closer than the minimum distance
		distance_to_next_step = distance_to_next_step - pathfinder.minimum_distance_to_target;
	}

	if (distance_to_next_step < movement_speed)
	{
		// We are one step away from the edge of the target, set that as the velocity and return success
		registry.velocities.get(pathfinder_entity) = distance_to_next_step * normalize(next_step - position);
		pathfinder.path.erase(pathfinder.path.begin());

		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}

	// Otherwise we still have some distance to go
	registry.velocities.get(pathfinder_entity) = movement_speed * normalize(next_step - position);

	return BEHAVIOUR_TREE_STATUS::RUNNING;
}
