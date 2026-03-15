#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"
#include "ecs/tiny_ecs.hpp"

class InteractableSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
};
