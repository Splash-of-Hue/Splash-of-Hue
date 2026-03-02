// stlib
#include <iostream>
#include <random>
#include <sstream>

#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
#include "creature_system/creature_behaviour_system.hpp"
#include "creature_system/creature_components.hpp"
#include <ai/pathfinding.hpp>

// todo: https://trello.com/c/Qj457xrS/105-2-creature-behaviour-system-a-pathfinding
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::aStarFollowTarget(Entity& follower, float elapsed_ms)
{
	if (direct_line_next_step(elapsed_ms, follower))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

// todo: https://trello.com/c/D5ie5Ml8/120-task-score-task-template
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::chargeAtTarget(Entity& charger, float elapsed_ms)
{
	// DEBUGGING std::cout << "chargeAtTarget" << std::endl;

	return BEHAVIOUR_TREE_STATUS::FAILURE;
}


BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::requestNewPath(Entity& pathfinder_entity, float elapsed_ms)
{
	Pathfinder& pathfinder = registry.pathfinders.get(pathfinder_entity);
	pathfinder.is_path_dirty = true;
	pathfinder.target = registry.players.entities[0];
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::requestNewRandomPath(Entity& pathfinder_entity, float elapsed_ms)
{
	Pathfinder& pathfinder = registry.pathfinders.get(pathfinder_entity);
	pathfinder.is_path_dirty = true;

	CurrentLevel& current_level = registry.currentLevels.get(pathfinder_entity);

	auto get_current_level = [&](Level& level) {return level.uid == current_level.level_uid; };
	auto& level_it = std::find_if(registry.levels.components.begin(), registry.levels.components.end(), get_current_level);
	
	if (level_it == registry.levels.components.end())
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	auto& quantized_level_space = level_it->quantized_space_data;


	bool found_spot = false;
	while (!found_spot)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> distLevel(0, quantized_level_space.size() - 1);

		auto& random_it = *std::next(quantized_level_space.begin(), distLevel(rng));

		found_spot = random_it.second.second;
		if (found_spot)
		{
			Entity entity;
			vec2& position = registry.positions.emplace(entity);

			position = vec2(random_it.second.first.x, random_it.second.first.y);
			pathfinder.target = entity;
		}
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::goToNextPathStep(Entity& pathfinder_entity, float elapsed_ms)
{
	vec2& position = registry.positions.get(pathfinder_entity);
	Pathfinder& pathfinder = registry.pathfinders.get(pathfinder_entity);

	// If there is no path, we are trying to follow a path that doesn't exist.
	// So return failure since we won't be pathfinding to anything and it was a failure to try.
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
	registry.velocities.get(pathfinder_entity) = movement_speed * normalize(next_step - position) * 5.f;
	return BEHAVIOUR_TREE_STATUS::RUNNING;
}
