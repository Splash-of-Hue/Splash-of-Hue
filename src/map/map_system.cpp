#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "loading/loading_constants.hpp"
#include "map_components.hpp"
#include "map_system.hpp"
#include <algorithm>
#include <map>
#include <vector>
#include <iostream>

void MapSystem::step(float elapsed_ms)
{
	auto& changeMapLevelRequestRegistry = registry.changeMapLevelRequests;

	// Most of the time there will not be load requests, so wait until there is
	if (changeMapLevelRequestRegistry.size() == 0)
	{
		return;
	}

	// We only care about the most recent load request
	const Entity& most_recent_request_entity = changeMapLevelRequestRegistry.entities.back();
	// If there is a load request, check the type and act appropriately
	ChangeMapLevelRequest& change_map_level_request = changeMapLevelRequestRegistry.get(most_recent_request_entity);
	Entity& moving_entity = change_map_level_request.moving_entity;
	// Update the moving entity, and get the old level from where the entity was before
	registry.positions.get(moving_entity) = change_map_level_request.arrival_position + change_map_level_request.arrival_offset;
	int previous_level_uid = registry.currentLevels.get(moving_entity).level_uid;
	int new_level_uid = change_map_level_request.new_level_uid;
	registry.currentLevels.get(moving_entity).level_uid = new_level_uid;
	// If this isn't a player, we are done. Don't load or unload based on it
	if (change_map_level_request.type != MAP_CHANGE_REQUEST_TYPE::PLAYER)
	{
		changeMapLevelRequestRegistry.clear();
		return;
	}
	// Mark the new level as visited 
	const auto new_level_match = [&](const Level& level) { return level.uid == new_level_uid; };
	auto new_level_it = std::find_if(registry.levels.components.begin(), registry.levels.components.end(), new_level_match);
	if (new_level_it != registry.levels.components.end())
	{
		new_level_it->is_visited = true;
	}
	// Load the new level. If the level is already loaded, it will just move the camera
	Entity load_request_entity;
	registry.loadRequests.insert(load_request_entity, { LOAD_TYPE::LOAD_LEVEL, new_level_uid });
	// For each level that was adjacent to the previous level, that isn't the current level, unload it
	const auto previous_level_match = [&](const Level& level) { return level.uid == previous_level_uid; };
	const auto& level_iterator = std::find_if(registry.levels.components.begin(), registry.levels.components.end(), previous_level_match);
	if (level_iterator != registry.levels.components.end())
	{
		for (int previous_adjacent_level_uid : level_iterator->neighbor_level_uids)
		{
			// Don't unload the currently loading level since it's a neighbour
			if (previous_adjacent_level_uid == new_level_uid)
			{
				continue;
			}

			Entity unload_request_entity;
			registry.loadRequests.insert(unload_request_entity, { LOAD_TYPE::UNLOAD_LEVEL, previous_adjacent_level_uid });
		}
	}

	Entity game_state_request_entity;
	registry.game_state_change_requests.insert(game_state_request_entity, { GAME_STATE::LOADING });
	// Once all load requests are taken care of, remove them
	changeMapLevelRequestRegistry.clear();
}
