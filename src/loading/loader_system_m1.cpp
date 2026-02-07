#include "loader_system.hpp"
#include "world_init.hpp"
//////////////////////////////////////////////////
///// Meant to be replaced with LDtk varient /////
//////////////////////////////////////////////////

struct M1ShapeData
{
	CREATURETYPE creatureType;
	Size size;
	vec2 position;
	vec4 color;
	vec2 velocity;
	// TODO: Add AI behaviour for creatures
};

struct M1TestData
{
	vec2 player_start_position; 
	std::vector<M1ShapeData> creatures;
	std::vector<M1ShapeData> light_toggles;
};

const std::vector<M1TestData> m1_test_data = {
	{
		{ 100, 100 },
		{
			// Three creatures
			{
				SAWFISH,
				{ 100, 100 }, // size
				{ 400, 120 }, // position
				{ 0.2f, 0.4f, 0, 1.0f }, // color
				vec2( 0.2f, 0.2f ) // velocity
			},
			{
				ANGLERFISH,
				{ 80, 20 }, // size
				{ 50, 20 }, // position
				{ 0, 1.0f, 0.6f, 1.0f  }, // color
				vec2(0.0f, 0.0f) // velocity
			},
			{
				SHRIMP,
				{ 50, 200 }, // size
				{ 800, 320 }, // position
				{ 0.4f, 0, 0.85f, 1.0f  }, // color
				vec2(0.0f, 0.0f) // velocity
			},
		},
		{
			// Two toggles
			{
				UNDEFINED,
				{ 100, 100 }, // size
				{ 600, 220 }, // position
				{ 1.0f, 0, 0, 1.0f  }, // color
				vec2(0.0f, 0.0f) // velocity
			},
			{
				UNDEFINED,
				{ 100, 100 }, // size
				{ 150, 500 }, // position
				{ 0, 0, 1.0f, 1.0f }, // color
				vec2(0.0f, 0.0f) // velocity
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
	Entity& player_entity = createCircle(level_data.player_start_position, 40.0f, { 1.0f, 0, 0, 1.0f });

	// Just for testing purposes, will be removed later 
	if (registry.players.entities.size() == 0) {
		registry.players.emplace(player_entity);
	}
	else
	{
		Entity& player_entity = registry.players.entities[0];
	}
	
	// TODO SnB9A38K: Move to player controller
	Motion& player_motion_component = registry.motions.emplace(player_entity);
	player_motion_component.position = vec2(0.0f, 0.0f); // stick it in the middle
	player_motion_component.velocity = vec2(0.0f, 0.0f);

	// Load each "creature", make a rectangle
	for (auto& creature : level_data.creatures)
	{
		Entity& creature_entity = createRectangle(creature.position, creature.size.width, creature.size.height, creature.color);
		Creature& creature_component = registry.creatures.emplace(creature_entity);
		creature_component.creatureType = creature.creatureType;
		Motion& motion_component = registry.motions.emplace(creature_entity);
		motion_component.position = creature.position;
		motion_component.velocity = creature.velocity;
	}

	// Load each "toggle", make a circle
	for (auto& toggle : level_data.light_toggles)
	{
		auto& toggle_entity = createCircle(toggle.position, toggle.size.width, toggle.color);
		registry.toggles.emplace(toggle_entity);
	}
}

void LoaderSystem::unload_level(int level_uid)
{
	// Do nothing, there is no behaviour for M1.
	// Later we will want to remove things from memory.
}
