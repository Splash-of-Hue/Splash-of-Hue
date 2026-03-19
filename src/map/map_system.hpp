#pragma once
#include "ecs/system.hpp"

class MapSystem : public SystemInterface
{
public:
	// This checks if there are any ChangeMapLevelRequests and if
	// there are it processes them.
	void step(float elapsed_ms);
};
