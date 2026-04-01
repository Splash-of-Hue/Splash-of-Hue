#pragma once

#include "common.hpp"
#include <vector>

struct Boid
{

};

// The overseer of all of its associated boids. 
struct BoidOverseer
{
	// The following factors can have their parameters adjusted to control how much the group swarms 
	
	// alignment: how much the boids want to align with the avg orientation of the local group
	float alignment_factor = 0.03f;

	// separation: avoiding being too close to neighbours
	float separation_factor = 6.5f;

	// cohesion: moving towards avg pos of local group
	float cohesion_factor = 0.06f;

	// Only allow boids to process boids that they can see (e.g. in front of them)
	float perception_radius = 50.0f;
	float perception_radius_squared = perception_radius * perception_radius;
	float fov_angle_deg = 180.0f;

	float min_speed = 30.0f;
	float max_speed = 100.0f;
	float max_force = 10.0f; 
	float steering_noise_factor = 0.05f;

	// We are updating BOIDs at less than 60fps
	// So need smoothing to avoid jitter. 
	float velocity_smoothing = 0.003f; 

	vec2 target_position = vec2(0);
	float target_factor = 1.0f;

	// Todo: BukbklVV 
	//Entity target;
	//std::map<Entity, Boid> boids;
};
