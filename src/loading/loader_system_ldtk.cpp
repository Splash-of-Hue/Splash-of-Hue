#include "creature_system/creature_creator.hpp"
#include "ecs/entity.hpp"
#include "ldtk_loader.hpp"
#include "loader_system.hpp"
#include "loading_constants.hpp"
#include "map/map_build_init.hpp"
#include <creature_system/creature_constants.hpp>
#include <iostream>
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Enum.hpp>
#include <LDtkLoader/Field.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tileset.hpp>
#include <unordered_map>
#include <renderer/render_constants.hpp>
#include <string>
#include <world_init.hpp>

void LoaderSystem::load_level(int level_uid)
{
	if (!project_loaded)
	{
		// load the file
		ldtk_project.loadFromFile(LDTK_PROJECT_PATH);
	}

	// If the level is already loaded, do nothing except change camera
	if (loaded_level_uids.count(level_uid) > 0)
	{
		for (Entity& level_entity : registry.levels.entities)
		{
			Level& level_component = registry.levels.get(level_entity);

			if (level_component.uid != level_uid)
			{
				continue;
			}

			vec2& level_position = registry.positions.get(level_entity);
			Entity change_camera_request;
			registry.changeCameraTargetRequests.insert(change_camera_request, { level_position, LDTK_CAMERA_ROOM_SPEED });
		}

		return;
	}

	// get a world
	const auto& world = ldtk_project.getWorld(LDTK_WORLD_ID);

	// get a level
	const ldtk::Level& level = world.getLevel(level_uid);
	loaded_level_uids.insert(level.uid);
	const ldtk::IntPoint& level_world_position = level.position;
	const ivec2 level_index = ivec2(level_world_position.x / level.size.x, level_world_position.y / level.size.y);
	Entity loading_level_entity;
	Level level_component = {
		level_uid,
		level_index
	};
	registry.levels.insert(loading_level_entity, level_component);

	if (LEVEL_TO_SCREEN_SCALE == vec2(0))
	{
		const ldtk::IntPoint& level_size = level.size;
		const float level_scale_x = WINDOW_WIDTH_PX / (float)level_size.x;
		const float level_scale_y = WINDOW_HEIGHT_PX / (float)level_size.y;
		LEVEL_TO_SCREEN_SCALE = vec2(level_scale_x, level_scale_y);
	}

	const vec2 level_world_center = vec2(level.position.x, level.position.y) * LEVEL_TO_SCREEN_SCALE + window_center();
	vec2& level_position = registry.positions.insert(loading_level_entity, level_world_center);
	Entity change_camera_request;
	registry.changeCameraTargetRequests.insert(change_camera_request, { level_position, LDTK_CAMERA_ROOM_SPEED });
	// get a layer
	const auto& level_layers = level.allLayers();

	for (const ldtk::Layer& layer : level_layers)
	{
		switch (layer.getType())
		{
			case ldtk::LayerType::AutoLayer:
			case ldtk::LayerType::IntGrid:
				// Not implemented yet
				continue;
			case ldtk::LayerType::Entities:
				load_entity_layer(layer);
				continue;
			case ldtk::LayerType::Tiles:
				load_tile_layer(layer);
				continue;
		}
	}

	// TODO rUJfFQQT: This is extremely inefficient. Use polygons to better improve this
	std::vector<Entity> level_wall_tile_entities;
	for (Entity level_tile_entity : registry.levelTiles.entities)
	{
		if (registry.levelTiles.get(level_tile_entity).level_uid == level_uid && registry.mapTiles.get(level_tile_entity).tile_type == MAP_TILE_TYPE::WALL)
		{
			level_wall_tile_entities.push_back(level_tile_entity);
		}
	}

	const int level_quantize_size = level.getLayer("Floor").getCellSize();
	const ivec2 quantize_grid_size = ivec2(WINDOW_WIDTH_PX / level_quantize_size, WINDOW_HEIGHT_PX / level_quantize_size);
	std::unordered_map<ivec2, std::pair<vec2, bool>> quantized_floor_data;

	for (int grid_x = 0; grid_x < quantize_grid_size.x; grid_x++)
	{
		for (int grid_y = 0; grid_y < quantize_grid_size.y; grid_y++)
		{
			// Get the world space coordinate
			vec2 quantize_world_position = vec2(
				(LEVEL_TO_SCREEN_SCALE.x * (level_world_position.x + level_quantize_size / 2)) + level_quantize_size * grid_x,
				(LEVEL_TO_SCREEN_SCALE.y * (level_world_position.y + level_quantize_size / 2)) + level_quantize_size * grid_y
			);

			// Check if this coordinate is in any of the wall tiles
			bool is_in_wall = false;

			for (Entity& wall_tile_entity : level_wall_tile_entities)
			{
				vec2& tile_position = registry.positions.get(wall_tile_entity);
				vec2& tile_size = registry.scales.get(wall_tile_entity);

				if (quantize_world_position.x < tile_position.x || quantize_world_position.x > tile_position.x + tile_size.x)
				{
					continue;
				}

				if (quantize_world_position.y < tile_position.y || quantize_world_position.y > tile_position.y + tile_size.y)
				{
					continue;
				}

				is_in_wall = true;
			}

			quantized_floor_data[{grid_x, grid_y}] = { quantize_world_position, !is_in_wall };
		}
	}

	registry.levels.get(loading_level_entity).quantized_space_data = quantized_floor_data;
}

void load_tile_layer(const ldtk::Layer& layer)
{
	const ldtk::IntPoint& level_size = layer.level->size;
	const ldtk::Tileset& layer_tileset = layer.getTileset();
	const ldtk::IntPoint& tileset_texture_size = layer_tileset.texture_size;
	const int& tile_size = layer_tileset.tile_size;

	// iterate on the tiles of the layer
	for (const auto& tile : layer.allTiles())
	{
		const ldtk::IntPoint& tile_position = tile.getPosition();
		const ldtk::IntPoint& tile_world_position = tile.getWorldPosition();
		const ldtk::IntRect& tile_texture_rect = tile.getTextureRect();
		vec4 destination_rect = {
			(tile_world_position.x + tile_size / 2) * LEVEL_TO_SCREEN_SCALE.x,
			(tile_world_position.y + tile_size / 2) * LEVEL_TO_SCREEN_SCALE.y,
			tile_size * LEVEL_TO_SCREEN_SCALE.x,
			tile_size * LEVEL_TO_SCREEN_SCALE.y
		};
		vec4 source_rect = {
			tile_texture_rect.x / (float)tileset_texture_size.x,
			tile_texture_rect.y / (float)tileset_texture_size.y,
			tile_size / (float)tileset_texture_size.x,
			tile_size / (float)tileset_texture_size.y
		};
		// TODO qEQqmtkO: Tile layers won't always be the map, move elsewhere
		MAP_TILE_TYPE map_tile_type = MAP_TILE_TYPE::DEFAULT;
		const auto& tile_tags = layer_tileset.getTileEnumTags(tile.tileId);
		
		for (const auto& tile_tag : tile_tags)
		{
			if (tile_tag.get().name == "Wall")
			{
				map_tile_type = map_tile_type | MAP_TILE_TYPE::WALL;
			}
		}

		// TODO mgydr9IJ: Get texture ID dynamically instead of always using ROOMS
		Entity& map_tile_entity = createMapTile(tile.tileId, TEXTURE_ASSET_ID::ROOMS, ivec2(tile_position.x, tile_position.y), destination_rect, source_rect, map_tile_type);
		registry.levelTiles.insert(map_tile_entity, { layer.level->uid });
	}
}

void load_entity_layer(const ldtk::Layer& layer)
{
	const std::string& layer_name = layer.getName();
	LOADING_LAYER_TYPE layer_type = layer_from_string(layer_name);

	switch (layer_type)
	{
		case LOADING_LAYER_TYPE::CREATURES:
			load_creatures(layer);
			return;
		case LOADING_LAYER_TYPE::FLOOR_INTERACTABLE:
			load_floor_interactables(layer);
			return;
		case LOADING_LAYER_TYPE::EXTRA_ENTITIES:
			load_extra_entities(layer);
			return;
		case LOADING_LAYER_TYPE::INVALID:
			std::cout << "Layer " << layer.getName() << ": Invalid layer type." << std::endl;
			return;
	}
}

void load_extra_entities(const ldtk::Layer& layer)
{
	// iterate on Enemy entities
	for (const ldtk::Entity& extra_entity : layer.allEntities())
	{
		const std::string extra_entity_type_string = extra_entity.getName();
		ENTITY_TYPE extra_entity_type = entity_type_from_string(extra_entity_type_string);

		switch (extra_entity_type)
		{
			case ENTITY_TYPE::INITIAL_POSITION:
				use_initial_player_position(extra_entity);
				continue;
			default:
				continue;
		}
	}
}

void use_initial_player_position(const ldtk::Entity& initial_player_position_entity)
{
	vec2& player_position = registry.positions.get(registry.players.entities[0]);
	const ldtk::IntPoint& player_world_position = initial_player_position_entity.getWorldPosition();
	player_position = vec2(player_world_position.x * LEVEL_TO_SCREEN_SCALE.x, player_world_position.y * LEVEL_TO_SCREEN_SCALE.y);
}

void load_creatures(const ldtk::Layer& layer)
{
	// iterate on Enemy entities
	for (const ldtk::Entity& ldtk_creature_entity : layer.allEntities())
	{
		const ldtk::Field<ldtk::EnumValue>& creature_enum_type = ldtk_creature_entity.getField<ldtk::EnumValue>("CreatureType");
		const CREATURETYPE& creature_type = creature_type_from_string(creature_enum_type.value().name);
		
		const ldtk::Field<ldtk::EnumValue>& color_state_enum_type = ldtk_creature_entity.getField<ldtk::EnumValue>("ColorState");
		const COLOUR_STATE& color_state = color_state_from_string(color_state_enum_type.value().name);
		
		const ldtk::IntPoint& creature_world_position = ldtk_creature_entity.getWorldPosition();
		const ldtk::IntPoint& creature_size = ldtk_creature_entity.getSize();
		const vec2 position = vec2(creature_world_position.x * LEVEL_TO_SCREEN_SCALE.x, creature_world_position.y * LEVEL_TO_SCREEN_SCALE.y);
		float width = creature_size.x * LEVEL_TO_SCREEN_SCALE.x;
		float height = creature_size.y * LEVEL_TO_SCREEN_SCALE.y;
		Entity creature_entity;

		switch (creature_type)
		{
			case CREATURETYPE::ANGLERFISH:
				creature_entity = createAnglerfish(position, width, height, color_state);
				break;

			case CREATURETYPE::CRAB:
				creature_entity = createCrab(position, width, height, color_state);
				break;

			case CREATURETYPE::MINNOWS:
				creature_entity = createMinnows(position, width, height, color_state);
				break;

			case CREATURETYPE::SAWFISH:
				creature_entity = createSawfish(position, width, height, color_state);
				break;

			case CREATURETYPE::SHRIMP:
				{
					const ldtk::Field<ldtk::EnumValue>& direction_enum_type = ldtk_creature_entity.getField<ldtk::EnumValue>("Direction");
					const DIRECTION& direction_state = direction_type_from_string(direction_enum_type.value().name);
					const ldtk::Field<int> arm_count = ldtk_creature_entity.getField<ldtk::FieldType::Int>("ArmCount");

					creature_entity = createShrimp(position, width, height, color_state, direction_state, arm_count.value());
				}
				break;

			case CREATURETYPE::WHALE:
				creature_entity = createWhale(position, width, height, color_state);
				break;

			default:
				std::cerr << "WARNING: unknown creature type, skipping." << std::endl;
				break;
		}

		registry.levelEntities.insert(creature_entity, { layer.level->uid });
	}
}

void load_floor_interactables(const ldtk::Layer& layer)
{
	// iterate on Enemy entities
	for (const ldtk::Entity& floor_interactable_entity : layer.allEntities())
	{
		const std::string floor_interactable_entity_type_string = floor_interactable_entity.getName();
		ENTITY_TYPE floor_interactable_entity_type = entity_type_from_string(floor_interactable_entity_type_string);

		switch (floor_interactable_entity_type)
		{
			case ENTITY_TYPE::SPAWN_POINT:
				load_spawn_points(floor_interactable_entity);
				continue;
			default:
				continue;
		}
	}
}

void load_spawn_points(const ldtk::Entity& spawn_point_entity)
{
	std::cout << spawn_point_entity.getName() << std::endl;
	const ldtk::EntityRef& other_spawn_point_entity_ref = spawn_point_entity.getField<ldtk::EntityRef>("ConnectedSpawnPoint").value();
	const auto& other_layer_grid_size = other_spawn_point_entity_ref->layer->getCellSize();
	const ldtk::IntPoint& position = spawn_point_entity.getWorldPosition();
	const ldtk::IntPoint& arrival_entity_position = other_spawn_point_entity_ref->getWorldPosition();
	const ldtk::IntPoint& arrival_grid_position = other_spawn_point_entity_ref->getField<ldtk::IntPoint>("ArrivalGridPosition").value();
	const ldtk::IntPoint& other_spawn_grid_position = other_spawn_point_entity_ref->getGridPosition();
	ivec2 other_spawn_grid_offset = ivec2(arrival_grid_position.x - other_spawn_grid_position.x, arrival_grid_position.y - other_spawn_grid_position.y);
	ivec2 other_spawn_offset = other_spawn_grid_offset * other_layer_grid_size;
	const vec2 world_arrival_point = vec2(arrival_entity_position.x + other_spawn_offset.x, arrival_entity_position.y + other_spawn_offset.y);

	createSpawnPoint(
		spawn_point_entity.layer->level->uid,
		other_spawn_point_entity_ref->layer->level->uid,
		vec2(position.x * LEVEL_TO_SCREEN_SCALE.x, position.y * LEVEL_TO_SCREEN_SCALE.y),
		vec2(world_arrival_point.x * LEVEL_TO_SCREEN_SCALE.x, world_arrival_point.y * LEVEL_TO_SCREEN_SCALE.y)
	);
}

void LoaderSystem::unload_level(int level_uid)
{
	// TODO Mh0WEPiT: Do nothing, this will be implemented later.
	// Later we will want to remove things from memory and registry.
}
