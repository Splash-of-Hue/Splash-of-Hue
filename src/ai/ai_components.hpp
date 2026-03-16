#pragma once

#include <vector>

// The overseer of all of its associated boids. 
struct Boid_Overseer
{
	// alignment: how much the boids want to align with the avg orientation of the local group
	float alignment_factor = 1.5f;

	// separation: avoiding being too close to neighbours
	float separation_factor = 1.0f;

	// cohesion: moving towards avg pos of local group
	float cohesion_factor = 1.0f;

	// Only allow boids to process boids that they can see (e.g. in front of them)
	float perception_radius = 50.0f;
	float perception_radius_squared = perception_radius * perception_radius;
	float fov_angle_deg = 120.0f;

	float max_speed = 100.0f;
	float max_force = 20.0f; 
	// Maximum number of neighbours a boid can process
	int max_neighbours = 100;

	Entity& target;

	std::map<Entity, Boid> boids;
};

struct Boid
{
	vec2 average_velocity = vec2(0.0f, 0.0f);
	vec2 average_neighbour_position = vec2(0.0f, 0.0f);
	vec2 proximity_force = vec2(0.0f, 0.0f);
};
