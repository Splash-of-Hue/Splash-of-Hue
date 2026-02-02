#pragma once

#include "common.hpp"
#include "render_system.hpp"
#include "ecs/registry.hpp"

class AISystem
{
public:
	void step(float elapsed_ms);
};