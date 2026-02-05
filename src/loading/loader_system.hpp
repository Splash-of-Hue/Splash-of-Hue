#pragma once
#include "ecs/registry.hpp"

class LoaderSystem
{
private:
	void load_level(int level_uid);
	void unload_level(int level_uid);

public:
	// This checks if there are any LoadRequests and if
	// there are it processes them with the ID.
	void step(float elapsed_ms);
};
