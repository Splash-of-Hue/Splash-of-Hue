#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "input_system.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms);

	void init(InputSystem* input_system);

	PhysicsSystem()
	{
	}

private:
	void handle_player_movement();

	// Input system reference for movement keys
	InputSystem* input_system;
};