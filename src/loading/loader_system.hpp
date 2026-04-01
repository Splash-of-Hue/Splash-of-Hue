#pragma once
#include "ecs/registry.hpp"
#include "ecs/system.hpp"

class LoaderSystem : public SystemInterface
{
private:
	// TODO: Re-assess, loading screen for M4
	void load_level(int level_uid, bool uses_loading_screen = false);
	void unload_level(int level_uid);

public:
	// This checks if there are any LoadRequests and if
	// there are it processes them with the ID.
	void step(float elapsed_ms);
};
