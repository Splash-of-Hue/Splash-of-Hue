#include "ai_system.hpp"

/*

Thanks to https://medium.com/better-programming/mastering-flock-simulation-with-boids-c-opengl-and-imgui-5a3ddd9cb958
for a thorough explanation of boids and the math behind it

*/
/*
// TODO: BukbklVV make more general (not minnows-specific)
// TODO: vXaDUAZZ optimize & split into helper functions
*/
void AISystem::updateBoids(float elapsed_ms)
{
	// Each boid group will have its own "overseer". For each level, this number should be relatively small
	for (Entity& boid_overseer_entity : registry.boidOverseers.entities)
	{
		BoidOverseer& boid_overseer = registry.boidOverseers.get(boid_overseer_entity);
		float separation_radius = boid_overseer.perception_radius * 0.2f;
		float separation_radius_sq = separation_radius * separation_radius;
		assert(registry.boid_minnows.size() > 0 && "there are no boids");

		// All of the boids will have the same visual field of view, so precompute.
		// We divide the fov_angle_deg by 2 to get the half angle because we want to check left and right without overcalculating
		float boid_fov_cos = glm::cos(glm::radians(boid_overseer.fov_angle_deg / 2));

		std::vector<vec2> goal_velocities;
		goal_velocities.reserve(registry.boid_minnows.size());

		for (Entity& boid_entity : registry.boid_minnows.entities)
		{
			const vec2& boid_position = registry.positions.get(boid_entity);
			vec2& boid_velocity = registry.velocities.get(boid_entity);
			const vec2 boid_forward_direction = glm::normalize(boid_velocity);

			vec2 sum_alignment = vec2(0, 0);
			vec2 sum_separation = vec2(0, 0);
			vec2 sum_cohesion = vec2(0, 0);
			int num_neighbours = 0;
			int num_neighbours_separation = 0;

			// Look at nearby boids, ignoring far away/outside field of view boids
			for (Entity& n_boid_entity : registry.boid_minnows.entities)
			{
				if (n_boid_entity == boid_entity)
					continue;

				const vec2& n_boid_position = registry.positions.get(n_boid_entity);
				vec2 difference = n_boid_position - boid_position;
				float distance_sq = glm::dot(difference, difference);

				// Apply random movement to boids that are too close to each other
				if (distance_sq < 0.001f)
				{
					float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
					sum_separation += vec2(cos(angle), sin(angle));
					num_neighbours_separation++;
					num_neighbours++;
					sum_alignment += registry.velocities.get(n_boid_entity);
					sum_cohesion += n_boid_position;
					continue;
				}

				// Ignore boids that are too far away
				if (distance_sq > boid_overseer.perception_radius_squared)
					continue;

				// Ignore boids that this boid can't see
				vec2 direction = glm::normalize(difference);
				// dot product here gets cosine angle, so if boid neighbour is in front, 
				// dot = 1, if it's beside it, dot = 0, if it's behind, dot = -1.
				if (glm::dot(boid_forward_direction, direction) <= boid_fov_cos)
					continue;

				// The neighbour boid is nearby without overlapping! Allow its forces to impact this boid
				num_neighbours++;

				// Only move boid away from neighbour if its too close
				if (separation_radius_sq > distance_sq)
				{
					float distance_sqrt = std::sqrt(distance_sq);
					sum_separation -= difference / distance_sqrt;
					num_neighbours_separation += 1;
				}

				sum_alignment += registry.velocities.get(n_boid_entity);
				sum_cohesion += n_boid_position;
			}

			// Get the average of all of the forces
			if (num_neighbours > 0)
			{
				sum_alignment = (sum_alignment / static_cast<float>(num_neighbours)) - boid_velocity;

				if (num_neighbours_separation > 0)
				{
					sum_separation = sum_separation / static_cast<float>(num_neighbours_separation);
				}

				sum_cohesion = (sum_cohesion / static_cast<float>(num_neighbours)) - boid_position;
			}
			else
			{
				// This boid is far away from the others and has no close neighbours, so move it towards the group centre.
				vec2 centre_of_other_boids = vec2(0, 0);

				for (Entity& n_boid_entity : registry.boid_minnows.entities)
				{
					if (n_boid_entity == boid_entity)
						continue;

					centre_of_other_boids += registry.positions.get(n_boid_entity);
				}

				// normalize by number of boids
				centre_of_other_boids /= static_cast<float>(registry.boid_minnows.size() - 1);
				vec2 difference = centre_of_other_boids - boid_position;

				sum_cohesion = glm::normalize(difference);
			}

			vec2 force = sum_cohesion * boid_overseer.cohesion_factor + sum_alignment * boid_overseer.alignment_factor + sum_separation * boid_overseer.separation_factor;

			// If a target is being considered, add a force towards the target
			if (boid_overseer.target_factor > 0) // no target
			{
				vec2 to_target = boid_overseer.target_position - boid_position;
				float target_dist = glm::length(to_target);
				if (target_dist > 0.001f)
					force += (to_target / target_dist) * boid_overseer.target_factor;
			}

			// Limit the steering force so there isn't any weird jittery movement
			float force_length = glm::length(force);
			if (force_length > boid_overseer.max_force && force_length != 0)
				force = (force / force_length) * boid_overseer.max_force;

			// Add steering noise so that they stop converging all to one point
			float noise_angle = rand() / static_cast<float>(RAND_MAX) * 2.0f * M_PI; // get a random angle [0, 2pi] ~ this could maybe be changed to be perlin noise so it's more smooth? something to ponder
			vec2 noise = vec2(cos(noise_angle), sin(noise_angle)); // convert angle to a unit vector
			force += noise * boid_overseer.steering_noise_factor;

			vec2 goal_velocity = boid_velocity + force * (elapsed_ms);

			// Clamp the speed so that the boids always have reasonable speed & use the forces mainly as direction
			float speed = glm::length(goal_velocity);
			if (speed > boid_overseer.max_speed)
			{
				goal_velocity = (goal_velocity / speed) * boid_overseer.max_speed;
			}
			else if (speed < boid_overseer.min_speed && speed > 0.001f)
			{
				goal_velocity = (goal_velocity / speed) * boid_overseer.min_speed;
			}
			else if (speed <= 0.001f)
			{
				goal_velocity = boid_forward_direction * boid_overseer.min_speed;
			}

			// Because we're updating at less than 60fps to save runtime, movement can be jittery
			// Compute how much of the gap between the current velocity and the goal velocity to close this frame using exponential smoothing
			float smoothing = 1.0f - glm::exp(-boid_overseer.velocity_smoothing * elapsed_ms);
			boid_velocity = glm::mix(boid_velocity, goal_velocity, smoothing);
		}
	}
}
