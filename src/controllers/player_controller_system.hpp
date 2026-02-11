#pragma once
#include "common.hpp"

class PlayerControllerSystem
{
private:
	vec2 player_movement;
	Entity create_player();

public:
	void step(float elapsed_ms);
};
