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
	CREATURETYPE creatureType;
	Size size;
	vec2 position;
	vec4 color;
	vec2 velocity;
	COLOUR_STATE color_state;
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
		{ 50, 50 },
		{
			// Creatures
			{
				SAWFISH,
				{ 60, 60 }, // size
				{ 50, 420 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2(0.2f, 0.2f), // velocity
				COLOUR_STATE::RED,
			},
			{
				SAWFISH,
				{ 60, 60 }, // size
				{ 280, 50 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2(0.2f, 0.2f), // velocity
				COLOUR_STATE::RED,
			},
			{
				SAWFISH,
				{ 60, 60 }, // size
				{ 340, 350 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2(0.2f, 0.2f), // velocity
				COLOUR_STATE::RED,
			},
			{
				SAWFISH,
				{ 60, 60 }, // size
				{ 340, 450 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2(0.2f, 0.2f), // velocity
				COLOUR_STATE::RED,
			},
			{
				SAWFISH,
				{ 60, 60 }, // size
				{ 340, 550 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2(0.2f, 0.2f), // velocity
				COLOUR_STATE::RED,
			},
			{
				SAWFISH,
				{ 60, 60 }, // size
				{ 340, 650 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2(0.2f, 0.2f), // velocity
				COLOUR_STATE::RED,
			},
			{
				SAWFISH,
				{ 60, 60 }, // size
				{ 700, 150 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2(0.2f, 0.2f), // velocity
				COLOUR_STATE::RED,
			},
		},
		{
			// Toggles
			{
				{ 50, 50 }, // size
				{ 50, 150 }, // position
				COLOUR_STATE::NEUTRAL, // state
				{ "wall_10" } // controlled wall IDs
			},
			{
				{ 50, 50 }, // size
				{ 320, 690 }, // position
				COLOUR_STATE::NEUTRAL, // state
				{ "wall_15" } // controlled wall IDs
			},
			{
				{ 50, 50 }, // size
				{ 540, 280 }, // position
				COLOUR_STATE::NEUTRAL, // state
				{ "wall_32" } // controlled wall IDs
			},
			{
				{ 50, 50 }, // size
				{ 850, 680 }, // position
				COLOUR_STATE::NEUTRAL, // state
				{ "wall_37" } // controlled wall IDs
			},
			{
				{ 50, 50 }, // size
				{ 650, 150 }, // position
				COLOUR_STATE::NEUTRAL, // state
				{ "wall_40" } // controlled wall IDs
			},
			{
				{ 50, 50 }, // size
				{ 850, 380 }, // position
				COLOUR_STATE::NEUTRAL, // state
				{ "wall_41" } // controlled wall IDs
			},
		},
		{
			// Walls
			{
				"wall_6", // ID
				{100, 20}, // size
				{50, 100}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_7", // ID
				{20, 150}, // size
				{100, 165}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_8", // ID
				{20, 700}, // size
				{180, 300}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_9", // ID
				{120, 20}, // size
				{50, 390}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_10", // ID
				{60, 20}, // size
				{140, 390}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_11", // ID
				{20, 220}, // size
				{100, 510}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_12", // ID
				{20, 400}, // size
				{280, 450}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_13", // ID
				{20, 100}, // size
				{380, 690}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_14", // ID
				{100, 20}, // size
				{320, 650}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_15", // ID
				{80, 20}, // size
				{230, 350}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_16", // ID
				{100, 20}, // size
				{240, 120}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_17", // ID
				{20, 120}, // size
				{280, 190}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_18", // ID
				{100, 20}, // size
				{320, 250}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_19", // ID
				{100, 20}, // size
				{320, 350}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_20", // ID
				{100, 20}, // size
				{320, 450}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_21", // ID
				{100, 20}, // size
				{320, 550}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_22", // ID
				{100, 20}, // size
				{320, 650}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_23", // ID
				{100, 20}, // size
				{420, 250}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_24", // ID
				{100, 20}, // size
				{420, 350}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_25", // ID
				{100, 20}, // size
				{420, 450}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_26", // ID
				{100, 20}, // size
				{420, 550}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_27", // ID
				{20, 600}, // size
				{470, 260}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_28", // ID
				{20, 100}, // size
				{580, 690}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_29", // ID
				{120, 20}, // size
				{450, 650}, // position
				COLOUR_STATE::RED // state
			},
			{
				"wall_30", // ID
				{230, 20}, // size
				{590, 550}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_31", // ID
				{20, 100}, // size
				{500, 590}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_32", // ID
				{20, 200}, // size
				{800, 640}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_33", // ID
				{20, 200}, // size
				{910, 640}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_34", // ID
				{20, 200}, // size
				{695, 440}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_35", // ID
				{20, 320}, // size
				{600, 250}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_36", // ID
				{20, 320}, // size
				{800, 380}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_37", // ID
				{110, 20}, // size
				{535, 210}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_38", // ID
				{200, 20}, // size
				{710, 210}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_39", // ID
				{200, 20}, // size
				{710, 100}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_40", // ID
				{20, 200}, // size
				{1010, 640}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_41", // ID
				{20, 130}, // size
				{910, 185}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_42", // ID
				{20, 200}, // size
				{910, 350}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_43", // ID
				{20, 150}, // size
				{910, 50}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_44", // ID
				{120, 20}, // size
				{860, 550}, // position
				COLOUR_STATE::NEUTRAL // state
			},
			{
				"wall_45", // ID
				{20, 550}, // size
				{1010, 270}, // position
				COLOUR_STATE::NEUTRAL // state
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
	// Now made each "creature" a sawfish
	for (auto& creature : level_data.creatures)
	{
		auto& creature_entity = createSawfish(creature.position, creature.size.width, creature.size.height, creature.color);
		Creature& creature_component = registry.creatures.emplace(creature_entity);
		creature_component.creatureType = creature.creatureType;
		registry.velocities.emplace(creature_entity, creature.velocity);
		auto& creature_collidable = registry.collidables.emplace(creature_entity);
		creature_collidable.inelastic = true;
		const ColourState creature_color_state = { creature.color_state };
		registry.colourStates.emplace(creature_entity, creature_color_state);
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
