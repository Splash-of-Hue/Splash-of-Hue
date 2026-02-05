#pragma once
#include "ecs/registry.hpp"

// /loading/room_generation_system.hpp
class LoaderSystem
{
private:
	// this is called by step and updates. Since loading might take more time than a frame.
	void load_level(int level_uid);
	void unload_level(int level_uid);

public:
	// This checks if there are any LoadRequests and if there are that
	// havent started processing it calls the load_room_elements with the ID.
	// It also checks if the RoomLoadRequests are finished and if it is,
	// removes the RoomLoadRequest entity.
	void step(float elapsed_ms);
};
