#include "ecs/registry.hpp"
#include "loader_system.hpp"
#include "map_builder/map_build_init.hpp"
#include "world_init.hpp"
#include <map>
#include <string>
#include <vector>
//////////////////////////////////////////////////
///// Meant to be replaced with LDtk varient /////
//////////////////////////////////////////////////

struct M1CreatureData
{
	Size size;
	vec2 position;
	vec4 color;
	// TODO: Add AI behaviour for creatures
};

struct M1ToggleData
{
	Size size;
	vec2 position;
	COLOUR_STATE color_state;
	std::vector<std::string> controlled_wall_ids;
};

struct M1WallData
{
	std::string id;
	Size size;
	vec2 position;
	COLOUR_STATE color_state;
};

struct M1TestData
{
	vec2 player_start_position;
	std::vector<M1CreatureData> creatures;
	std::vector<M1ToggleData> light_toggles;
	std::vector<M1WallData> coloured_walls;
};

const std::vector<M1TestData> m1_test_data = {
	{
		{ 100, 100 },
		{
			// Three creatures
			{
				{ 100, 100 }, // size
				{ 400, 120 }, // position
				{ 0.2f, 0.4f, 0, 1.0f } // color
			},
			{
				{ 80, 20 }, // size
				{ 50, 20 }, // position
				{ 0, 1.0f, 0.6f, 1.0f  } // color
			},
			{
				{ 50, 200 }, // size
				{ 800, 320 }, // position
				{ 0.4f, 0, 0.85f, 1.0f  } // color
			},
		},
		{
			// Two toggles
			{
				{ 50, 50 }, // size
				{ 600, 50 }, // position
				COLOUR_STATE::BLUE, // state
				{ "wall_1" } // controlled wall IDs
			},
			{
				{ 100, 100 }, // size
				{ 700, 400 }, // position
				COLOUR_STATE::RED, // state
				{ "wall_2", "wall_3"} // controlled wall IDs
			},
		},
		{
			// Walls
			{
				"wall_1", // ID
				{20, 100}, // size
				{600, 100}, // position
				COLOUR_STATE::BLUE // state
			},
			{
				"wall_2", // ID
				{100, 20}, // size
				{640, 150}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_3", // ID
				{20, 100}, // size
				{680, 210}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_4", // ID
				{100, 20}, // size
				{640, 270}, // position
				COLOUR_STATE::BLUE // state
			},
			{
				"wall_5", // ID
				{20, 200}, // size
				{600, 370}, // position
				COLOUR_STATE::BLUE // state
			},
		},
	}
};

void LoaderSystem::load_level(int level_uid)
{
	assert(level_uid == 0 && "For M1, we are using mock data, and the unique ID must be 0");
	auto& level_data = m1_test_data[level_uid];
	Entity level_entity;
	registry.levels.insert(level_entity, { level_uid });

	// For now, the player is a red circle of size 40
	auto& player_entity = registry.players.entities[0];
	auto& player_position = registry.positions.get(player_entity);
	player_position = level_data.player_start_position;

	// As we make walls, store their entities in a vector as you can't store Entities in a map
	std::vector<Entity> wall_entities = {};
	std::map<std::string, int> wall_id_to_entity_map = {};

	// Load each "creature", make a rectangle
	for (auto& creature : level_data.creatures)
	{
		auto& creature_entity = createRectangle(creature.position, creature.size.width, creature.size.height, creature.color);
		registry.creatures.emplace(creature_entity);
	}

	// Make walls and store their entities to use when making toggles
	for (auto& wall : level_data.coloured_walls)
	{
		auto& colored_wall_entity = createWall(wall.position, wall.size.width, wall.size.height, wall.color_state);

		wall_entities.push_back(colored_wall_entity);
		wall_id_to_entity_map[wall.id] = wall_entities.size() - 1;
	}

	// Load each "toggle", make a circle
	for (auto& toggle : level_data.light_toggles)
	{
		std::vector<Entity> walls_to_toggle = {};

		for (std::string wall_id : toggle.controlled_wall_ids)
		{
			walls_to_toggle.push_back(wall_entities[wall_id_to_entity_map[wall_id]]);
		}

		createToggle(toggle.position, toggle.size.width, toggle.color_state, walls_to_toggle);
	}
}

void LoaderSystem::unload_level(int level_uid)
{
	// Do nothing, there is no behaviour for M1.
	// Later we will want to remove things from memory.
}
