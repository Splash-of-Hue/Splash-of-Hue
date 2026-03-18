#pragma once

#include "common.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"
#include <vector>

class AISystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
	std::vector<vec2> generatePath(Entity pathfinder_entity);
};
