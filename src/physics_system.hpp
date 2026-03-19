#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem : public SystemInterface
{
public:
	
	void step(float elapsed_ms);
	PhysicsSystem()
	{
	}

private:
	void checkIfInRadius();
	void checkIfCollidingRadius(Entity& entity, std::vector<Entity>& radius_list);
	void handle_movement(float elapsed_ms);
};
