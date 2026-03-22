#pragma once

#include "ai_constants.hpp"
#include "common.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"

#include <numbers>
#include <random>
#include <vector>

class AISystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
	std::vector<vec2> generatePath(Entity pathfinder_entity);
private:
	// Boids
	void updateBoids(float elapsed_ms);

	float time_accumulator = 0.0f;
	const float update_interval = 750.0f; // ms, updating at 45fps
};
