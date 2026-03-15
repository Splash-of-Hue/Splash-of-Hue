#pragma once

struct Boid_Overseer
{
	// alignment: how much the boids want to align with the avg orientation of the local group
	float alignment_factor;
	// separation: avoiding being too close to neighbours
	float separation_factor;
	// cohesion: moving towards avg pos of local group
	float cohesion_factor;
};

struct Boid
{

};
