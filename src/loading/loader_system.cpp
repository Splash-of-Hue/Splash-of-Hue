#include "loader_system.hpp"

void LoaderSystem::step(float elapsed_ms)
{
	auto& loadRequestRegistry = registry.loadRequests;

	// Most of the time there will not be load requests, so wait until there is
	if (loadRequestRegistry.size() == 0)
	{
		return;
	}

	// If there is a load request, check the type and act appropriately
	for (Entity& entity : loadRequestRegistry.entities)
	{
		LoadRequest& load_request = loadRequestRegistry.get(entity);

		switch (load_request.load_type)
		{
			case LOAD_TYPE::LOAD_LEVEL:
				// TODO: Temporary change for M4, for camera
				load_level(load_request.uid, load_request.uses_loading_screen);
				break;
			case LOAD_TYPE::UNLOAD_LEVEL:
				unload_level(load_request.uid);
				break;
			default:
				assert(false && "Trying to load invalid load type.");
		}
	}

	Entity game_state_request_entity;
	registry.game_state_change_requests.insert(game_state_request_entity, { GAME_STATE::PLAYING });
	// Once all load requests are taken care of, remove them
	loadRequestRegistry.clear();
}
