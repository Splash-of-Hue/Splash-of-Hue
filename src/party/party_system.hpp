#pragma once
#include "ecs/system.hpp"

class PartySystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
};
