#include "ai_system.hpp"
/*

Thanks to https://medium.com/better-programming/mastering-flock-simulation-with-boids-c-opengl-and-imgui-5a3ddd9cb958
for a thorough explanation of boids and the math behind it

*/


void AISystem::updateBoids(float elapsed_ms)
{
	// Each boid group will have its own "overseer". For each level, this number should be relatively small
	for (Entity& boid_overseer_entity : registry.boidOverseers.entities)
	{
		Boid_Overseer& boid_overseer = registry.boidOverseers.get(boid_overseer_entity);
		float separation_radius = boid_overseer.perception_radius * 0.3f;
		float separation_radius_sq = separation_radius * separation_radius;
		assert(registry.boid_minnows.size() > 0 && "there are no boids");

		// All of the boids will have the same visual field of view, so precompute.
		// We divide the fov_angle_deg by 2 to get the half angle because we want to check left and right without overcalculating
		float boid_fov_cos = glm::cos(glm::radians(boid_overseer.fov_angle_deg / 2));

		for (Entity& boid_entity : registry.boid_minnows.entities)
		{
			// Need to make a copy to ensure that the .id() check later passes. Not an issue becuase we're not emplacing the entity anywhere
			const vec2& boid_position = registry.positions.get(boid_entity);
			vec2& boid_velocity = registry.velocities.get(boid_entity);
			const vec2 boid_forward_direction = glm::normalize(boid_velocity);

			// Reset values
			vec2 sum_alignment = vec2(0, 0);
			vec2 sum_separation = vec2(0, 0);
			vec2 sum_cohesion = vec2(0, 0);
			int num_neighbours = 0;

			// Look at nearby boids, ignoring far away/outside field of view boids
			for (Entity& n_boid_entity : registry.boid_minnows.entities)
			{
				if (n_boid_entity == boid_entity)
					continue;

				// Ignore boids that are too far away
				const vec2 n_boid_position = registry.positions.get(n_boid_entity);
				vec2 difference = n_boid_position - boid_position;
				float distance_sq = glm::dot(difference, difference);
				if (distance_sq > boid_overseer.perception_radius_squared)
					continue;


				// Ignore boids that this boid can't see
				vec2 direction = glm::normalize(difference);

				// dot product here gets cosine angle, so if boid neighbour is in front, dot = 1, if it's beside it, dot = 0, if it's behind, dot = -1.
				if (glm::dot(boid_forward_direction, direction) <= boid_fov_cos)
					continue;

				// This boid is nearby, add its values to the forces sums
				num_neighbours += 1;

				// Only move away from neighbour if you're too close
				if (distance_sq > 0.001f && separation_radius_sq > distance_sq)
				{
					float distance_sqrt = std::sqrt(distance_sq);
					sum_separation -= difference / distance_sqrt;
				}
				// If the boids are on top of each other, move them randomly 
				else
				{
					float angle = (float)rand() / RAND_MAX * 2.0f * M_PI;
					vec2 anglevec = vec2(cos(angle), sin(angle));
					sum_separation += anglevec;
				}

				sum_alignment += registry.velocities.get(n_boid_entity);
				sum_cohesion += n_boid_position;
			}

			// Get the average of all of the forces
			if (num_neighbours > 0)
			{
				sum_alignment = (sum_alignment / static_cast<float>(num_neighbours)) - boid_velocity;

				sum_separation = sum_separation / static_cast<float>(num_neighbours);

				sum_cohesion = (sum_cohesion / static_cast<float>(num_neighbours)) - boid_position;
			}
			else
			{
				// This boid is far away from the others, so move it towards the group centre. 
				// This should very rarely, if ever, run
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
			
			// Limit the steering force so there isn't any weird movement
			float force_length = glm::length(force);
			if (force_length > boid_overseer.max_force)
				force = (force / force_length) * boid_overseer.max_force;
			// maybe add some steering noise

			boid_velocity += force * elapsed_ms; // sophia note: this may need to be changed to be the delta time stuff @geetika was doing for physics? will need to test

			float speed = glm::length(boid_velocity);
			if (speed > boid_overseer.max_speed)
				boid_velocity = (boid_velocity / speed) * boid_overseer.max_speed;
		}

	}
}

