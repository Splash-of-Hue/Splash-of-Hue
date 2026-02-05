#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:

	void step(float elapsed_ms);

	PhysicsSystem()
	{
	}

private:
	void handle_player_movement();

};

