#pragma once
#include "ecs/system.hpp"

class PlayerControllerSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
};
