#include <iostream>
#include "ai_system.hpp"
#include "world_init.hpp"
#include "pathfinding.hpp"
#include "common.hpp"

void AISystem::step(float elapsed_ms)
{
	for (const Entity& pathfinder_entity : registry.pathfinders.entities)
	{
		Pathfinder& pathfinder = registry.pathfinders.get(pathfinder_entity);

		if (!pathfinder.is_path_dirty)
		{
			continue;
		}

		// Path is dirty, so update the path
		std::vector<vec2> new_path = generatePath(pathfinder_entity);
		pathfinder.path = new_path;
		pathfinder.is_path_dirty = false;
	}
}

std::vector<vec2> AISystem::generatePath(Entity pathfinder_entity)
{
	Pathfinder& pathfinder = registry.pathfinders.get(pathfinder_entity);

	switch (pathfinder.type)
	{
		case PATHFINDING_TYPE::BEST:
		{
			// TODO rUJfFQQT: This is currently very expensive to run
			// Find the current level and then find the closest point for both this entity and the target
			vec2 position = registry.positions.get(pathfinder_entity);
			float closest_grid_value = INFINITE;
			ivec2 grid_position;

			vec2 target_position = registry.positions.get(pathfinder.target);
			float closest_target_grid_value = INFINITE;
			ivec2 target_grid_position;

			std::unordered_map<ivec2, bool> valid_position_map;
			std::unordered_map<ivec2, vec2> world_space_map;

			const int level_uid = registry.levelEntities.get(pathfinder_entity).level_uid;

			for (Level& level : registry.levels.components)
			{
				if (level.uid == level_uid)
				{
					for (auto& level_quantized_space : level.quantized_space_data)
					{
						valid_position_map.insert({ level_quantized_space.first, level_quantized_space.second.second });
						world_space_map.insert({ level_quantized_space.first, level_quantized_space.second.first });

						const float from_dist = distance(level_quantized_space.second.first, position);
						const float target_dist = distance(level_quantized_space.second.first, target_position);

						if (from_dist < closest_grid_value)
						{
							closest_grid_value = from_dist;
							grid_position = level_quantized_space.first;
						}

						if (target_dist < closest_target_grid_value)
						{
							closest_target_grid_value = target_dist;
							target_grid_position = level_quantized_space.first;
						}
					}
				}
			}

			std::vector<ivec2> quantized_path = a_star(grid_position, target_grid_position, valid_position_map);
			std::vector<vec2> path;

			for (ivec2 quantized_position : quantized_path)
			{
				path.push_back(world_space_map[quantized_position]);
			}

			return path;
		}
		case PATHFINDING_TYPE::DIRECT:
		{
			// Case to be handled
		}
	}

	return {};
}
