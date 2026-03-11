#include "creature_system/creature_constants.hpp"
#include "creature_system/creature_creator.hpp"
#include "ecs/entity.hpp"
#include "interactables/interactable_creator.hpp"
#include "ldtk_loader.hpp"
#include "loader_system.hpp"
#include "loading_constants.hpp"
#include "map/map_build_init.hpp"
#include "renderer/render_constants.hpp"
#include <algorithm>
#include <iostream>
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Enum.hpp>
#include <LDtkLoader/Field.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tileset.hpp>
#include <string>
#include <unordered_map>

void LoaderSystem::load_level(int level_uid)
{
	if (!project_loaded)
	{
		// load the file
		ldtk_project.loadFromFile(LDTK_PROJECT_PATH);
	}
	std::cout << "Loading level ID " << level_uid << std::endl;

	const auto level_match = [&](const Level& level) { return level.uid == level_uid; };
	const auto level_entity_match = [&](Entity& entity) { return registry.levels.get(entity).uid == level_uid; };
	auto level_iterator = std::find_if(registry.levels.components.begin(), registry.levels.components.end(), level_match);
	bool level_has_been_loaded_before = level_iterator != registry.levels.components.end();

	// If the level had been loaded previously, we might be done if it's still loaded
	if (level_has_been_loaded_before && level_iterator->is_loaded)
	{
		// If the level is already loaded, do nothing except change camera and set background color
		Entity& level_entity = *std::find_if(registry.levels.entities.begin(), registry.levels.entities.end(), level_entity_match);
		vec2& level_position = registry.positions.get(level_entity);
		Entity change_camera_request;
		registry.changeCameraTargetRequests.insert(change_camera_request, { level_position, LDTK_CAMERA_ROOM_SPEED });
		registry.screenStates.components[0].background_color = registry.levels.get(level_entity).background_color;

		return;
	}

	// get a world
	const auto& world = ldtk_project.getWorld(LDTK_WORLD_ID);

	// get a level
	const ldtk::Level& level = world.getLevel(level_uid);
	const ldtk::IntPoint& level_world_position = level.position;
	const ivec2 level_index = ivec2(level_world_position.x / level.size.x, level_world_position.y / level.size.y);

	if (LEVEL_TO_SCREEN_SCALE == vec2(0))
	{
		const ldtk::IntPoint& level_size = level.size;
		const float level_scale_x = WINDOW_WIDTH_PX / (float)level_size.x;
		const float level_scale_y = WINDOW_HEIGHT_PX / (float)level_size.y;
		LEVEL_TO_SCREEN_SCALE = vec2(level_scale_x, level_scale_y);
	}

	if (!level_has_been_loaded_before)
	{
		// Level has not been loaded before, make a new entity and set the position
		Entity loading_level_entity;
		Level& level_component = registry.levels.insert(loading_level_entity, { level_uid, level_index });
		const vec2 level_world_center = scale_to_level(vec2(level.position.x, level.position.y)) + window_center();
		vec2& level_position = registry.positions.insert(loading_level_entity, level_world_center);
		level_component.background_color = vec4(level.bg_color.r, level.bg_color.g, level.bg_color.b, level.bg_color.a) / 255.0f;
	}

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
	for (Entity& level_tile_entity : registry.levelTiles.entities)
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
			const vec2 quantize_world_position = scale_to_level(
				vec2(level_world_position.x + level_quantize_size, level_world_position.y + level_quantize_size) / 2.0f
			) + vec2(grid_x, grid_y) * float(level_quantize_size);

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

	// Load the quantized space and set that the level is now loaded
	Entity& level_entity = *std::find_if(registry.levels.entities.begin(), registry.levels.entities.end(), level_entity_match);
	Level& level_component = registry.levels.get(level_entity);
	level_component.quantized_space_data = quantized_floor_data;
	level_component.is_loaded = true;
	// Use level background color to update screen background
	registry.screenStates.components[0].background_color = level_component.background_color;
	// Make the camera request at the end of loading
	Entity change_camera_request;
	registry.changeCameraTargetRequests.insert(change_camera_request, { registry.positions.get(level_entity), LDTK_CAMERA_ROOM_SPEED});
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
		const vec2 destination_position = scale_to_level(vec2(tile_world_position.x, tile_world_position.y) + vec2(tile_size) / 2.0f);
		const vec2 destination_size = scale_to_level(vec2(tile_size));
		vec4 destination_rect = {
			destination_position.x,
			destination_position.y,
			destination_size.x,
			destination_size.y
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
				set_initial_player_position(extra_entity);
				continue;
			case ENTITY_TYPE::WALL:
				load_wall(extra_entity);
				continue;
			case ENTITY_TYPE::MOSS_WALL:
				load_moss_door(extra_entity);
				continue;
			default:
				continue;
		}
	}
}

void set_initial_player_position(const ldtk::Entity& initial_player_position_entity)
{
	// Otherwise, this is the first time the level is loaded so use the initial position
	Entity& player_entity = registry.players.entities[0];

	// Player is already loaded, don't need to set initial position
	if (registry.players.get(player_entity).is_loaded)
	{
		return;
	}

	registry.currentLevels.get(player_entity).level_uid = initial_player_position_entity.layer->level->uid;
	const ldtk::IntPoint& initial_player_world_position = initial_player_position_entity.getWorldPosition();
	vec2 initial_player_position = scale_to_level(vec2(initial_player_world_position.x, initial_player_world_position.y));
	registry.positions.get(player_entity) = initial_player_position;
	registry.players.get(player_entity).is_loaded = true;
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
		const vec2 position = scale_to_level(vec2(creature_world_position.x, creature_world_position.y));
		const vec2 size = scale_to_level(vec2(creature_size.x, creature_size.y));
		Entity creature_entity;
		const int level_uid = layer.level->uid;

		switch (creature_type)
		{
			case CREATURETYPE::ANGLERFISH:
				// TODO nsTuVKMT: Check if this specific one already exists since it can move
				creature_entity = createAnglerfish(level_uid, position, width, height, color_state);
				registry.currentLevels.insert(creature_entity, { level_uid });
				break;

			case CREATURETYPE::CRAB:
				creature_entity = createCrab(level_uid, position, width, height, color_state);
				break;

			case CREATURETYPE::MINNOWS:
				creature_entity = createMinnows(level_uid, position, width, height, color_state);
				break;

			case CREATURETYPE::SAWFISH:
				// TODO nsTuVKMT: Check if this specific one already exists since it can move
				creature_entity = createSawfish(level_uid, position, width, height, color_state);
				registry.currentLevels.insert(creature_entity, { level_uid });
				break;

			case CREATURETYPE::SHRIMP:
			{
				const ldtk::Field<ldtk::EnumValue>& direction_enum_type = ldtk_creature_entity.getField<ldtk::EnumValue>("Direction");
				const DIRECTION& direction_state = direction_type_from_string(direction_enum_type.value().name);
				const ldtk::Field<int> arm_count = ldtk_creature_entity.getField<ldtk::FieldType::Int>("ArmCount");

				creature_entity = createShrimp(level_uid, position, width, height, color_state, direction_state, arm_count.value());
			}
			break;

			case CREATURETYPE::WHALE:
				creature_entity = createWhale(level_uid, position, width, height, color_state);
				break;

			default:
				std::cerr << "WARNING: unknown creature type, skipping." << std::endl;
				break;
		}
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
			case ENTITY_TYPE::TELEPORTER:
				load_teleporter(floor_interactable_entity);
				continue;
			case ENTITY_TYPE::MOSS_TOGGLE:
				load_moss_toggle(floor_interactable_entity);
				continue;
			case ENTITY_TYPE::COLOUR_ORB:
				load_colour_orb(floor_interactable_entity);
				continue;
			case ENTITY_TYPE::BOX:
				load_box(floor_interactable_entity);
				continue;
			default:
				continue;
		}
	}
}

void load_teleporter(const ldtk::Entity& teleporter_entity)
{
	std::cout << teleporter_entity.getName() << std::endl;
	const ldtk::EntityRef& other_teleporter_entity_ref = teleporter_entity.getField<ldtk::EntityRef>("ConnectedTeleporter").value();
	const auto& other_layer_grid_size = other_teleporter_entity_ref->layer->getCellSize();
	const ldtk::IntPoint& position = teleporter_entity.getWorldPosition();
	const ldtk::IntPoint& size = teleporter_entity.getSize();
	const ldtk::IntPoint& arrival_entity_position = other_teleporter_entity_ref->getWorldPosition();
	bool has_defined_arrival_position = !other_teleporter_entity_ref->getField<ldtk::IntPoint>("ArrivalGridPosition").is_null();
	ivec2 other_teleporter_grid_offset = ivec2(0);

	if (has_defined_arrival_position)
	{
		const ldtk::IntPoint& arrival_grid_position = other_teleporter_entity_ref->getField<ldtk::IntPoint>("ArrivalGridPosition").value();
		const ldtk::IntPoint& other_teleporter_grid_position = other_teleporter_entity_ref->getGridPosition();
		other_teleporter_grid_offset = ivec2(arrival_grid_position.x - other_teleporter_grid_position.x, arrival_grid_position.y - other_teleporter_grid_position.y);
	}

	ivec2 other_teleporter = other_teleporter_grid_offset * other_layer_grid_size;
	const vec2 world_arrival_point = vec2(arrival_entity_position.x + other_teleporter.x, arrival_entity_position.y + other_teleporter.y);
	int level_uid = teleporter_entity.layer->level->uid;
	int connected_level_uid = other_teleporter_entity_ref->layer->level->uid;
	const ldtk::Field<ldtk::EnumValue>& offset_direction_field = other_teleporter_entity_ref->getField<ldtk::EnumValue>("OffsetDirection");
	const DIRECTION& offset_direction = direction_type_from_string(offset_direction_field.value().name);

	createTeleporter(
		level_uid,
		connected_level_uid,
		scale_to_level(vec2(position.x, position.y)),
		{ size.x, size.y },
		scale_to_level(world_arrival_point),
		offset_direction
	);

	// Add the connected level UIDs as neighbours to the current level (which must exist at this point)
	const auto level_match = [&](const Level& level) { return level.uid == level_uid; };
	Level& level = *std::find_if(registry.levels.components.begin(), registry.levels.components.end(), level_match);
	level.neighbor_level_uids.insert(connected_level_uid);
}

void load_moss_toggle(const ldtk::Entity& moss_toggle_entity)
{
	const ldtk::IntPoint& position = moss_toggle_entity.getWorldPosition();
	const ldtk::IntPoint& size = moss_toggle_entity.getSize();
	const vec2 world_position = scale_to_level(vec2(position.x, position.y));
	const vec2 scaled_size = scale_to_level(vec2(size.x, size.y));
	const ldtk::ArrayField<ldtk::EntityRef>& wall_entity_ref_fields = moss_toggle_entity.getField<ldtk::FieldType::ArrayEntityRef>("ConnectedWalls");
	std::vector<std::string> connected_wall_ids;
	const ldtk::Field<ldtk::EnumValue>& color_state_enum_type = moss_toggle_entity.getField<ldtk::EnumValue>("ColorState");
	const COLOUR_STATE& color_state = color_state_from_string(color_state_enum_type.value().name);
	
	for (const ldtk::Field<ldtk::EntityRef>& wall_entity_ref_field : wall_entity_ref_fields)
	{
		const ldtk::EntityRef wall_entity_ref = wall_entity_ref_field.value();
		connected_wall_ids.push_back(wall_entity_ref.entity_iid.str());
	}

	createMossToggle(moss_toggle_entity.layer->level->uid, world_position, scaled_size, color_state, connected_wall_ids);
}

void load_moss_door(const ldtk::Entity& moss_door_entity)
{
	const ldtk::IntPoint& position = moss_door_entity.getWorldPosition();
	const ldtk::IntPoint& size = moss_door_entity.getSize();
	const vec2 world_position = scale_to_level(vec2(position.x, position.y));
	const vec2 scaled_size = scale_to_level(vec2(size.x, size.y));
	std::string moss_door_id = moss_door_entity.iid.str();
	const ldtk::Field<ldtk::EnumValue>& color_state_enum_type = moss_door_entity.getField<ldtk::EnumValue>("ColorState");
	COLOUR_STATE color_state = color_state_from_string(color_state_enum_type.value().name);

	const auto find_toggle = [&](Entity& entity)
	{
		std::vector<std::string>& moss_toggle_ids = registry.mossToggles.get(entity).connected_wall_ids;
		return std::find(moss_toggle_ids.begin(), moss_toggle_ids.end(), moss_door_id) != moss_toggle_ids.end();
	};
	const auto toggle_component_iterator = std::find_if(registry.mossToggles.entities.begin(), registry.mossToggles.entities.end(), find_toggle);

	// If there are any toggles that exist for this door, use the toggle color
	if (toggle_component_iterator != registry.mossToggles.entities.end())
	{
		color_state = registry.colourStates.get(*toggle_component_iterator).curr_colour_state;
	}

	createMossWall(moss_door_entity.layer->level->uid, moss_door_id, world_position, scaled_size, color_state);
}

void load_wall(const ldtk::Entity& wall_entity)
{
	std::cout << wall_entity.getName() << std::endl;
	const ldtk::IntPoint& wall_world_position = wall_entity.getWorldPosition();
	const ldtk::IntPoint& wall_size = wall_entity.getSize();
	const vec2 position = scale_to_level(vec2(wall_world_position.x, wall_world_position.y));
	const vec2 size = scale_to_level(vec2(wall_size.x, wall_size.y));

	Entity& wall = createWall(wall_entity.layer->level->uid, position, size.x, size.y);
}

void load_colour_orb(const ldtk::Entity& colour_orb_entity)
{
	const ldtk::Field<ldtk::EnumValue>& color_state_field = colour_orb_entity.getField<ldtk::EnumValue>("ColorState");
	const COLOUR_STATE colour_state = color_state_from_string(color_state_field.value().name);

	// Don't spawn it if the player has the abilities
	const Player& player = registry.players.components[0];
	if (player.blue_unlocked && colour_state == COLOUR_STATE::BLUE || player.red_unlocked && colour_state == COLOUR_STATE::RED)
	{
		return;
	}

	const ldtk::IntPoint& world_pos = colour_orb_entity.getWorldPosition();
	const ldtk::IntPoint& entity_size = colour_orb_entity.getSize();
	const vec2 position = scale_to_level(vec2(world_pos.x, world_pos.y));
	const float radius = scale_to_level(vec2(entity_size.x)).x * 0.5f;
	Entity orb = createColourOrb(position, radius, colour_state);
	registry.levelEntities.insert(orb, { colour_orb_entity.layer->level->uid });
}

void load_box(const ldtk::Entity& box_entity)
{
	const ldtk::IntPoint& world_pos = box_entity.getWorldPosition();
	const ldtk::IntPoint& entity_size = box_entity.getSize();
	vec2 position = vec2(world_pos.x, world_pos.y) + vec2(entity_size.x, entity_size.y) / 2.0f;
	position = scale_to_level(position);
	const vec2 size = scale_to_level(vec2(entity_size.x, entity_size.y));
	Entity box = createBox(position, size.x, size.y);
	registry.levelEntities.insert(box, { box_entity.layer->level->uid });
}

void LoaderSystem::unload_level(int level_uid)
{
	// Try to find the level component
	const auto level_match = [&](const Level& level) { return level.uid == level_uid; };
	auto& level_iterator = std::find_if(registry.levels.components.begin(), registry.levels.components.end(), level_match);
	std::cout << "Unloading level ID " << level_uid;

	// First check if the level has been previously loaded or if it is loaded at all
	if (level_iterator == registry.levels.components.end() || !level_iterator->is_loaded)
	{
		std::cout << " failed, either level hasn't been loaded or is already unloaded." << std::endl;
		// Either the level was never loaded or is already unloaded
		return;
	}

	std::cout << "." << std::endl;

	// Get every tile from that level
	for (Entity& level_tile_entity : registry.levelTiles.entities)
	{
		// Unload the tiles if it is in the unloading level
		if (registry.levelTiles.get(level_tile_entity).level_uid == level_uid)
		{
			registry.remove_all_components_of(level_tile_entity);
		}
	}

	// Get every entity from that level and every entity from other levels that moved to that level
	for (Entity& level_entities_entity : registry.levelEntities.entities)
	{
		bool entity_moves_through_levels = registry.currentLevels.has(level_entities_entity);

		if (entity_moves_through_levels)
		{
			// This entity moves around, check where it is currently.
			const int& entity_current_level = registry.currentLevels.get(level_entities_entity).level_uid;

			// If the current level is not the one being unloaded, move on
			if (entity_current_level != level_uid)
			{
				continue;
			}
		}

		// Otherwise, it can't leave the level, so check as usual if it's the right level
		if (!entity_moves_through_levels && registry.levelEntities.get(level_entities_entity).level_uid != level_uid)
		{
			continue;
		}

		// The entity is either not in the level we are unloading if it moves
		// or it's not part of the level we are unloading
		registry.remove_all_components_of(level_entities_entity);
	}

	// Keep the map data but remove from loaded levels (is generated, is not loaded)
	level_iterator->is_loaded = false;
}
