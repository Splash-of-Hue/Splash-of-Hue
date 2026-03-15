#pragma once
#include "common.hpp"
#include "ecs/system.hpp"

class PlayerControllerSystem : public SystemInterface
{
private:
	vec2 player_movement;
	Entity create_player();

public:
	void step(float elapsed_ms);
};
