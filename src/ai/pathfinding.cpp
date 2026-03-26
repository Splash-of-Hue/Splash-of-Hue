#include "creature_system/creature_components.hpp"
#include "glm/gtx/hash.hpp"
#include "pathfinding.hpp"
#include <ecs/registry.hpp>
#include <unordered_set>
#include <vector>

// Bare bones direct pathfinding 
void direct_line_next_step(float elapsed_ms, Entity& follower, Entity& target)
{
	assert(registry.pathfinders.has(follower) && "The follower creature isn't registered as a pathfinder");
	Pathfinder& pathfinder_component = registry.pathfinders.get(follower);

	// TODO: REFACTOR as part of pathfinding tasks in later milestones SO THIS IS GENERAL FOR ANY "TARGET". Whatever entity's ColourComponent triggers it
	assert(registry.positions.has(target) && "Target doesn't have a position");
	assert(registry.positions.has(follower) && "Pathfinder doesn't have a position");
	assert(registry.velocities.has(follower) && "Pathfinder doesn't have a velocity");

	const vec2& target_position = registry.positions.get(target);
	vec2& follower_position = registry.positions.get(follower);
	vec2& follower_velocity = registry.velocities.get(follower);

	// Get the vector to the player
	float dx = target_position.x - follower_position.x;
	float dy = target_position.y - follower_position.y;

	// Calculate and compare squares bec they're cheaper to avoid sqrt
	float distance_squared = dx * dx + dy * dy;
	float minDistToTarget_squared = pathfinder_component.minimum_distance_to_target * pathfinder_component.minimum_distance_to_target;

	// If too close to the target, stop
	if (distance_squared <= minDistToTarget_squared)
	{
		follower_velocity = { 0.0f, 0.0f };
		return;
	}

	// Sqrt only when moving for normalization
	float distance = std::sqrt(distance_squared);
	float norm_x = dx / distance;
	float norm_y = dy / distance;

	// Slow down when approaching the player so there isn't an abrupt stop
	float slow_radius = pathfinder_component.minimum_distance_to_target * 2;;
	float max_speed = pathfinder_component.max_speed * elapsed_ms * 0.01;
	if (distance < slow_radius)
	{
		float t = (distance - pathfinder_component.minimum_distance_to_target) / (slow_radius - pathfinder_component.minimum_distance_to_target);
		t = std::clamp(t, 0.f, 1.f);
		max_speed = max_speed * t;
	}
	follower_velocity = { norm_x * max_speed, norm_y * max_speed };
}

std::vector<ivec2> a_star(ivec2 start_position_grid, ivec2 end_position_grid, std::unordered_map<ivec2, bool> valid_grid_coordinates)
{
	std::vector<ivec2> path = {};
	// The open set is the positions to travel to
	std::unordered_set<ivec2> open_set = { start_position_grid };
	// The cloased set is the positions that have been traveled to, this is kept for debugging purposes later
	std::unordered_set<ivec2> closed_set;
	// Heuristic and weight function, nothing unique in this grid-based version
	auto h = [](ivec2 position) { return 1; };
	auto d = [](ivec2 current, ivec2 neighbour) { return 1; };
	// Set up maps
	std::unordered_map<ivec2, ivec2> previous_best_position_map;
	std::unordered_map<ivec2, float> g_score_map;
	std::unordered_map<ivec2, float> f_score_map;
	g_score_map[start_position_grid] = 0;
	f_score_map[start_position_grid] = h(start_position_grid);

	while (open_set.size() > 0)
	{
		// Reset current position so we use one that's in the open set
		ivec2 current_pos = ivec2(0);

		// Find the best next position to try
		for (ivec2 position : open_set)
		{
			if (f_score_map.count(current_pos) == 0 || f_score_map[position] < f_score_map[current_pos])
			{
				current_pos = position;
			}
		}

		if (current_pos == end_position_grid)
		{
			// Contruct path and return
			path.push_back(current_pos);

			while (previous_best_position_map.count(current_pos) > 0)
			{
				current_pos = previous_best_position_map[current_pos];
				path.insert(path.begin(), current_pos);
			}

			return path;
		}

		// Remove from the open set and add to the closed set
		open_set.erase(current_pos);
		closed_set.insert(current_pos);
		// Set up the neighboring nodes
		std::vector<ivec2> neighbors;
		auto get_neighbors = [](ivec2 position) { return std::vector<ivec2> { position + ivec2(0, 1), position - ivec2(0, 1), position + ivec2(1, 0), position - ivec2(1, 0) }; };

		for (ivec2& neighbor : get_neighbors(current_pos))
		{
			// If the neighbor is not in the grid or is in the grid but not valid, continue
			if (!valid_grid_coordinates[neighbor])
			{
				continue;
			}

			float tentative_g_score = g_score_map[current_pos] + d(current_pos, neighbor);

			// If the G score isn't registered yet or is better than other routes, make note
			if (g_score_map.count(neighbor) == 0 || tentative_g_score < g_score_map[neighbor])
			{
				previous_best_position_map[neighbor] = current_pos;
				g_score_map[neighbor] = tentative_g_score;
				f_score_map[neighbor] = tentative_g_score + h(neighbor);

				if (open_set.count(neighbor) == 0)
				{
					open_set.insert(neighbor);
				}
			}
		}
	}

	return path;
}
