#pragma once
#include "ecs/system.hpp"

class MenuControllerSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
};
