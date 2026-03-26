#pragma once
#include "ecs/entity.hpp"
#include "loading_constants.hpp"
#include <vector>

// Used to load in new data dynamically instead of at start of the game
struct LoadRequest
{
	// The type of thing that needs loading. Could be a level, music, etc.
	LOAD_TYPE load_type;
	// The unique ID of whatever needs to be loaded
	int uid;
};

// When something needs to be unloaded, it sometimes has other entities, we should unload them too.
struct UnloadRelatedEntities
{
	std::vector<Entity> entities;
};
