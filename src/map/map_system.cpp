#include "map_components.hpp"
#include "map_system.hpp"
#include "loading/loading_constants.hpp"
#include <ecs/registry.hpp>

void MapSystem::step(float elapsed_ms)
{
	auto& changeMapLevelRequestRegistry = registry.changeMapLevelRequests;

	// Most of the time there will not be load requests, so wait until there is
	if (changeMapLevelRequestRegistry.size() == 0)
	{
		return;
	}

	// If there is a load request, check the type and act appropriately
	for (Entity& entity : changeMapLevelRequestRegistry.entities)
	{
		ChangeMapLevelRequest& change_map_level_request = changeMapLevelRequestRegistry.get(entity);

		// Update the player
		Entity& player_entity = registry.players.entities[0];
		registry.positions.get(player_entity) = change_map_level_request.new_player_position;

		// Load level, this will do nothing is the level is already loaded
		Entity load_request_entity;
		registry.loadRequests.insert(load_request_entity, { LOAD_TYPE::LOAD_LEVEL, change_map_level_request.new_level_uid });
	}

	// Once all load requests are taken care of, remove them
	changeMapLevelRequestRegistry.clear();
}
