#include "loader_system.hpp"
#include <iostream>
void LoaderSystem::step(float elapsed_ms)
{
	auto& loadRequestRegistry = registry.loadRequests;

	// Most of the time there will not be load requests, so wait until there is
	if (loadRequestRegistry.size() == 0)
	{
		return;
	}

	// If there is a load request, check the type and act appropriately
	for (auto& entity : loadRequestRegistry.entities)
	{
		auto& load_request = loadRequestRegistry.get(entity);

		switch (load_request.load_type)
		{
			case LOAD_TYPE::LOAD_LEVEL:
				load_level(load_request.uid);
				break;
			case LOAD_TYPE::UNLOAD_LEVEL:
				unload_level(load_request.uid);
				break;
			default:
				assert(false && "Trying to load invalid load type.");
		}
	}

	// Once all load requests are taken care of, remove them
	loadRequestRegistry.clear();
}
