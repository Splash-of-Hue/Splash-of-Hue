#include "loading/loading_constants.hpp"
#include "map_components.hpp"
#include "map_system.hpp"
#include <algorithm>
#include <ecs/registry.hpp>
#include <vector>

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
	// Update the player, and get the old level from where the player was before
	Entity& player_entity = registry.players.entities[0];
	registry.positions.get(player_entity) = change_map_level_request.new_player_position;
	int previous_level_uid = registry.currentLevels.get(player_entity).level_uid;
	int new_level_uid = change_map_level_request.new_level_uid;
	registry.currentLevels.get(player_entity).level_uid = new_level_uid;
	// Load the new level. If the level is already loaded, it will jsut move the camera
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

	// Once all load requests are taken care of, remove them
	changeMapLevelRequestRegistry.clear();
}
