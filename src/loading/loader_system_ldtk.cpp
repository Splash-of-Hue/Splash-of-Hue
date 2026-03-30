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
		project_loaded = true;
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
		level_component.depth = level.depth;
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
		if (registry.levelTiles.get(level_tile_entity).level_uid == level_uid && registry.mapTiles.get(level_tile_entity).tile_type == TILE_TYPE::WALL)
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
			// Get the world space coordinate and offset by half the quantize size to center it
			vec2 quantize_world_position = scale_to_level(vec2(level_world_position.x, level_world_position.y) + vec2(level_quantize_size) / 2.0f);
			// Offset by the grid number scaled by the cell size
			quantize_world_position += vec2(grid_x, grid_y) * float(level_quantize_size);

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

	// Mark visited if this is the initial load or if the player is currently in this level
	if (is_initial_load)
	{
		level_component.is_visited = true;
		is_initial_load = false;
	}
	else if (!level_has_been_loaded_before && !registry.players.entities.empty())
	{
		Entity player_entity = registry.players.entities[0];
		if (registry.currentLevels.has(player_entity) &&
			registry.currentLevels.get(player_entity).level_uid == level_uid)
		{
			level_component.is_visited = true;
		}
	}
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

		// Get the tile data based on which tile layer thi sis
		const std::pair<TEXTURE_ASSET_ID, std::map<std::string, TILE_TYPE>> tile_layer_data = get_tile_layer_data_from_string(layer.getName());
		const TEXTURE_ASSET_ID tile_layer_texture = tile_layer_data.first;
		const std::map<std::string, TILE_TYPE> tile_layer_tags = tile_layer_data.second;
		// Get tile tags to loop through
		TILE_TYPE map_tile_type = TILE_TYPE::DEFAULT;
		const auto& tile_tags = layer_tileset.getTileEnumTags(tile.tileId);

		for (const auto& tile_tag : tile_tags)
		{
			const std::string tag_name = tile_tag.get().name;
			const TILE_TYPE tile_type = tile_layer_tags.at(tag_name);
			map_tile_type = map_tile_type | TILE_TYPE::WALL;
		}

		// TODO mgydr9IJ: Get texture ID dynamically instead of always using ROOMS
		Entity& map_tile_entity = createMapTile(tile.tileId, tile_layer_texture, ivec2(tile_position.x, tile_position.y), destination_rect, source_rect, map_tile_type);
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
		case LOADING_LAYER_TYPE::MINIMAP:
			laod_minimap(layer);
			return;
		case LOADING_LAYER_TYPE::DECORATIONS:
			load_decorations(layer);
			return;
		case LOADING_LAYER_TYPE::INVALID:
			std::cout << "Layer " << layer.getName() << ": Invalid layer type." << std::endl;
			return;
	}
}

void laod_minimap(const ldtk::Layer& layer)
{

	// iterate on Enemy entities
	for (const ldtk::Entity& extra_entity : layer.allEntities())
	{
		const std::string extra_entity_type_string = extra_entity.getName();
		ENTITY_TYPE extra_entity_type = entity_type_from_string(extra_entity_type_string);

		switch (extra_entity_type)
		{
			case ENTITY_TYPE::MINIMAP_REFERENCE:
				load_minimap_reference(extra_entity);
				continue;
			case ENTITY_TYPE::MINIMAP_HALLWAY:
				load_minimap_hallway(extra_entity);
				continue;
			default:
				continue;
		}
	}
}

void load_decorations(const ldtk::Layer& layer)
{
	// iterate on Enemy entities
	for (const ldtk::Entity& decoration_entity : layer.allEntities())
	{
		const std::string decoration_entity_type_string = decoration_entity.getName();
		ENTITY_TYPE decoration_entity_type = entity_type_from_string(decoration_entity_type_string);

		switch (decoration_entity_type)
		{
			case ENTITY_TYPE::PUZZLE_BUBBLE_FAKE:
				load_puzzle_bubble_fake(decoration_entity);
				continue;
			default:
				continue;
		}
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
				initialize_player(extra_entity);
				continue;
			case ENTITY_TYPE::WALL:
				load_wall(extra_entity);
				continue;
			case ENTITY_TYPE::MOSS_WALL:
				load_moss_door(extra_entity);
				continue;
			case ENTITY_TYPE::PRESSURE_PLATE_WALL:
				load_pressure_plate_walls(extra_entity);
				continue;
			case ENTITY_TYPE::PUZZLE_WALL:
				load_puzzle_wall(extra_entity);
				continue;
			case ENTITY_TYPE::PUZZLE_SOLUTION:
				load_puzzle_solution(extra_entity);
				continue;
			default:
				continue;
		}
	}
}

void initialize_player(const ldtk::Entity& initial_player_position_entity)
{
	int level_uid = initial_player_position_entity.layer->level->uid;
	const ldtk::IntPoint& initial_player_world_position = initial_player_position_entity.getWorldPosition();
	vec2 initial_player_position = scale_to_level(vec2(initial_player_world_position.x, initial_player_world_position.y));
	// Request a player be made
	Entity player_create_request_entity;
	registry.playerCreateRequests.insert(player_create_request_entity, { level_uid, initial_player_position });
}

void load_creatures(const ldtk::Layer& layer)
{
	CreatureCreator creatureCreator;

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
				for (Entity& e : registry.unloadables.entities) {
					vec2& pos = registry.unloadables.get(e).initial_position;
					//account for roundoff error
					if (abs(pos.x - position.x) < 10 && abs(pos.y - position.y) < 10) {
						//don't load this, it was loaded
						return;
					}
				}
				creature_entity = creatureCreator.createAnglerfish(level_uid, position, size.x, size.y, color_state);
				registry.currentLevels.insert(creature_entity, { level_uid });
				break;

			case CREATURETYPE::CRAB:
			{
				for (Entity& e : registry.unloadables.entities) {
					vec2& pos = registry.unloadables.get(e).initial_position;
					//account for roundoff error
					if (abs(pos.x - position.x) < 10 && abs(pos.y - position.y) < 10) {
						//don't load this, it was loaded
						return;
					}
				}
				const ldtk::Field<ldtk::EnumValue>& direction_enum_type = ldtk_creature_entity.getField<ldtk::EnumValue>("Direction");
				const DIRECTION& direction_state = direction_type_from_string(direction_enum_type.value().name);
				creature_entity = creatureCreator.createCrab(level_uid, position, size.x, size.y, color_state, direction_state);
			}
			break;

			case CREATURETYPE::MINNOWS:
				creature_entity = creatureCreator.createMinnows(level_uid, position, size.x, size.y, color_state);
				break;

			case CREATURETYPE::SAWFISH:
				// TODO nsTuVKMT: Check if this specific one already exists since it can move
				for (Entity& e : registry.unloadables.entities) {
					vec2& pos = registry.unloadables.get(e).initial_position;
					//account for roundoff error
					if (abs(pos.x - position.x) < 10 && abs(pos.y - position.y) < 10) {
						//don't load this, it was loaded
						return;
					}
				}
				creature_entity = creatureCreator.createSawfish(level_uid, position, size.x, size.y, color_state);
				registry.currentLevels.insert(creature_entity, { level_uid });
				break;

			case CREATURETYPE::SHRIMP:
			{
				const ldtk::Field<ldtk::EnumValue>& direction_enum_type = ldtk_creature_entity.getField<ldtk::EnumValue>("Direction");
				const DIRECTION& direction_state = direction_type_from_string(direction_enum_type.value().name);
				const ldtk::Field<int> arm_count = ldtk_creature_entity.getField<ldtk::FieldType::Int>("ArmCount");

				creature_entity = creatureCreator.createShrimp(level_uid, position, size.x, size.y, color_state, direction_state, arm_count.value());
			}
			break;

			case CREATURETYPE::WHALE:
				creature_entity = creatureCreator.createWhale(level_uid, position, size.x, size.y, color_state);
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
			case ENTITY_TYPE::MUSHROOM:
				load_mushroom(floor_interactable_entity);
				continue;
			case ENTITY_TYPE::PRESSURE_PLATE:
				load_pressure_plate(floor_interactable_entity);
				continue;
			case ENTITY_TYPE::PUZZLE_BUBBLE:
				load_puzzle_bubble(floor_interactable_entity);
				continue;
			case ENTITY_TYPE::CONTROL_PNG_ENTITY:
				load_control_pictures(floor_interactable_entity);
				continue;
			case ENTITY_TYPE::SCRATCHED_FLOOR_HINT_PNG_ENTITY:
				load_floor_hint_pictures(floor_interactable_entity);
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
	for (Entity& e : registry.unloadables.entities) {
		vec2& pos = registry.unloadables.get(e).initial_position;
		//account for roundoff error
		if (abs(pos.x - world_position.x) < 10 && abs(pos.y - world_position.y) < 10) {
			//don't load this, it was loaded
			return;
		}
	}
	const ldtk::ArrayField<ldtk::EntityRef>& wall_entity_ref_fields = moss_toggle_entity.getField<ldtk::FieldType::ArrayEntityRef>("ConnectedWalls");
	std::vector<std::string> connected_wall_ids;
	
	for (const ldtk::Field<ldtk::EntityRef>& wall_entity_ref_field : wall_entity_ref_fields)
	{
		const ldtk::EntityRef wall_entity_ref = wall_entity_ref_field.value();
		connected_wall_ids.push_back(wall_entity_ref.entity_iid.str());
	}

	createMossToggle(moss_toggle_entity.layer->level->uid, world_position, scaled_size, connected_wall_ids);
}

void load_moss_door(const ldtk::Entity& moss_door_entity)
{
	const ldtk::IntPoint& position = moss_door_entity.getWorldPosition();
	const ldtk::IntPoint& size = moss_door_entity.getSize();
	const vec2 world_position = scale_to_level(vec2(position.x, position.y));
	const vec2 scaled_size = scale_to_level(vec2(size.x, size.y));
	for (Entity& e : registry.unloadables.entities) {
		vec2& pos = registry.unloadables.get(e).initial_position;
		//account for roundoff error
		if (abs(pos.x - world_position.x) < 10 && abs(pos.y - world_position.y) < 10) {
			//don't load this, it was loaded
			return;
		}
	}
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

void load_pressure_plate(const ldtk::Entity& pressure_plate_entity)
{
	const ldtk::IntPoint& position = pressure_plate_entity.getWorldPosition();
	const ldtk::IntPoint& size = pressure_plate_entity.getSize();
	const vec2 world_position = scale_to_level(vec2(position.x, position.y));
	const vec2 scaled_size = scale_to_level(vec2(size.x, size.y));
	const ldtk::ArrayField<ldtk::EntityRef>& wall_entity_ref_fields = pressure_plate_entity.getField<ldtk::FieldType::ArrayEntityRef>("ConnectedWalls");
	std::vector<std::string> connected_wall_ids;

	for (const ldtk::Field<ldtk::EntityRef>& wall_entity_ref_field : wall_entity_ref_fields)
	{
		const ldtk::EntityRef wall_entity_ref = wall_entity_ref_field.value();
		connected_wall_ids.push_back(wall_entity_ref.entity_iid.str());
	}

	createPressurePlate(pressure_plate_entity.layer->level->uid, world_position, scaled_size, connected_wall_ids);
}

void load_pressure_plate_walls(const ldtk::Entity& pressure_plate_entity)
{
	const ldtk::IntPoint& position = pressure_plate_entity.getWorldPosition();
	const ldtk::IntPoint& size = pressure_plate_entity.getSize();
	const vec2 world_position = scale_to_level(vec2(position.x, position.y));
	const vec2 scaled_size = scale_to_level(vec2(size.x, size.y));
	std::string pressure_plate_wall_id = pressure_plate_entity.iid.str();
	bool is_open = false;
	for (Entity& e : registry.unloadables.entities) {
		vec2& pos = registry.unloadables.get(e).initial_position;
		//account for roundoff error
		if (abs(pos.x - world_position.x) < 1 && abs(pos.y - world_position.y) < 1) {
			//don't load this, it was broken
			return;
		}
	}
	const auto find_pressure_plate = [&](Entity& entity)
		{
			std::vector<std::string>& pressure_plate_ids = registry.pressurePlates.get(entity).connected_wall_ids;
			return std::find(pressure_plate_ids.begin(), pressure_plate_ids.end(), pressure_plate_wall_id) != pressure_plate_ids.end();
		};
	const auto pressure_plate_component_iterator = std::find_if(registry.pressurePlates.entities.begin(), registry.pressurePlates.entities.end(), find_pressure_plate);

	// If there are any pressure plates that are currently being pressed, ensure the is_open state is set correctly 
	if (pressure_plate_component_iterator != registry.pressurePlates.entities.end())
	{
		is_open = registry.pressurePlates.get(*pressure_plate_component_iterator).is_pressed;
	}

	createPressurePlateWall(pressure_plate_entity.layer->level->uid, pressure_plate_wall_id, world_position, scaled_size, is_open);
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
	for (Entity& e : registry.unloadables.entities) {
		vec2& pos = registry.unloadables.get(e).initial_position;
		//account for roundoff error
		if (abs(pos.x - position.x) < 10 && abs(pos.y - position.y) < 10) {
			//don't load this, it was broken
			return;
		}
	}
	const vec2 size = scale_to_level(vec2(entity_size.x, entity_size.y));
	Entity box = createBox(position, size.x, size.y);
	registry.levelEntities.insert(box, { box_entity.layer->level->uid });
}

void load_mushroom(const ldtk::Entity& mushroom_entity)
{
	const ldtk::IntPoint& world_pos = mushroom_entity.getWorldPosition();
	const ldtk::IntPoint& size = mushroom_entity.getSize();

	const ldtk::Field<ldtk::EnumValue>& color_state_field = mushroom_entity.getField<ldtk::EnumValue>("ColorState");
	const COLOUR_STATE colour_state = color_state_from_string(color_state_field.value().name);
	//todo: https://trello.com/c/qB3PaZDl/247-05-create-ldtk-size-general-enum
	const ldtk::Field<ldtk::EnumValue>& mushroom_size_field = mushroom_entity.getField<ldtk::EnumValue>("MushroomSize");
	const MUSHROOM_SIZE mushroom_size = mushroom_size_from_string(mushroom_size_field.value().name);

	const vec2 position = vec2(world_pos.x * LEVEL_TO_SCREEN_SCALE.x, world_pos.y * LEVEL_TO_SCREEN_SCALE.y);
	const float width = size.x * LEVEL_TO_SCREEN_SCALE.x;

	for (Entity& e : registry.unloadables.entities) {
		vec2& pos = registry.unloadables.get(e).initial_position;
		//account for roundoff error
		if (abs(pos.x - position.x) < 10 && abs(pos.y - position.y) < 10) {
			//don't load this
			return;
		}
	}
	Entity mushroom = createMushroom(position, width, colour_state, mushroom_size);

	registry.levelEntities.insert(mushroom, { mushroom_entity.layer->level->uid });
}

void load_minimap_reference(const ldtk::Entity& minimap_reference_entity)
{
	const int level_uid = minimap_reference_entity.layer->level->uid;
	const ldtk::IntPoint& world_pos = minimap_reference_entity.getWorldPosition();
	const ldtk::IntPoint& size = minimap_reference_entity.getSize();

	const auto level_match = [&](const Level& level) { return level.uid == level_uid; };
	auto level_it = std::find_if(
		registry.levels.components.begin(),
		registry.levels.components.end(), level_match);

	if (level_it == registry.levels.components.end()) {
		return;
	}

	level_it->floor_ldtk_position = { world_pos.x, world_pos.y };
	level_it->floor_ldtk_size = { size.x, size.y };
}

void load_minimap_hallway(const ldtk::Entity& minimap_hallway_entity)
{
	const int level_uid = minimap_hallway_entity.layer->level->uid;
	const ldtk::IntPoint& world_pos = minimap_hallway_entity.getWorldPosition();
	const ldtk::IntPoint& size = minimap_hallway_entity.getSize();

	const auto level_match = [&](const Level& level) { return level.uid == level_uid; };
	auto level_it = std::find_if(
		registry.levels.components.begin(),
		registry.levels.components.end(), level_match);

	if (level_it == registry.levels.components.end()) {
		return;
	}

	level_it->hallway_ldtk_rects.push_back({
		{ world_pos.x, world_pos.y },
		{ size.x, size.y }
		});
	
}

void load_control_pictures(const ldtk::Entity& control_picture_entity)
{
	const ldtk::Field<ldtk::EnumValue>& control_type_field = control_picture_entity.getField<ldtk::EnumValue>("ControlPictureType");
	const CONTROL_PNG_TYPE control_png_type = control_png_type_from_string(control_type_field.value().name);
	const ldtk::IntPoint& world_pos = control_picture_entity.getWorldPosition();
	const ldtk::IntPoint& entity_size = control_picture_entity.getSize();
	const vec2 position = vec2(world_pos.x * LEVEL_TO_SCREEN_SCALE.x, world_pos.y * LEVEL_TO_SCREEN_SCALE.y);
	const float width = entity_size.x * LEVEL_TO_SCREEN_SCALE.x;
	const float height = entity_size.y * LEVEL_TO_SCREEN_SCALE.y;
	Entity control_entity = createFloorPictureControls(position, width, height, control_png_type);
	registry.levelEntities.insert(control_entity, { control_picture_entity.layer->level->uid });
}

void load_floor_hint_pictures(const ldtk::Entity& floor_hint_png_entity)
{
	const ldtk::Field<ldtk::EnumValue>& floor_hint_type_field = floor_hint_png_entity.getField<ldtk::EnumValue>("FloorHintPictureType");
	const HINT_PNG_TYPE floor_hint_png_type = floor_hint_png_type_from_string(floor_hint_type_field.value().name);
	const ldtk::IntPoint& world_pos = floor_hint_png_entity.getWorldPosition();
	const ldtk::IntPoint& entity_size = floor_hint_png_entity.getSize();
	const vec2 position = vec2(world_pos.x * LEVEL_TO_SCREEN_SCALE.x, world_pos.y * LEVEL_TO_SCREEN_SCALE.y);
	const float width = entity_size.x * LEVEL_TO_SCREEN_SCALE.x;
	const float height = entity_size.y * LEVEL_TO_SCREEN_SCALE.y;
	Entity floor_hint_entity = createFloorPictureHint(position, width, height, floor_hint_png_type);
	registry.levelEntities.insert(floor_hint_entity, { floor_hint_png_entity.layer->level->uid });
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
		if (!registry.unloadables.has(level_entities_entity)) {
			registry.remove_all_components_of(level_entities_entity);
		}
		
	}

	// Keep the map data but remove from loaded levels (is generated, is not loaded)
	level_iterator->is_loaded = false;
}

void load_puzzle_bubble(const ldtk::Entity& puzzle_bubble_entity)
{
	std::cout << puzzle_bubble_entity.getName() << std::endl;
	const ldtk::IntPoint& world_position = puzzle_bubble_entity.getWorldPosition();
	const vec2 position = scale_to_level(vec2(world_position.x, world_position.y));
	const ldtk::Field<ldtk::EnumValue>& color_solution_field = puzzle_bubble_entity.getField<ldtk::EnumValue>("ColorSolution");
	const COLOUR_STATE colour_solution = color_state_from_string(color_solution_field.value().name);
	createPuzzleBubble(puzzle_bubble_entity.layer->level->uid, puzzle_bubble_entity.iid.str(), position, colour_solution);
}

void load_puzzle_bubble_fake(const ldtk::Entity& puzzle_bubble_fake_entity)
{
	std::cout << puzzle_bubble_fake_entity.getName() << std::endl;
	const ldtk::IntPoint& world_position = puzzle_bubble_fake_entity.getWorldPosition();
	const vec2 position = scale_to_level(vec2(world_position.x, world_position.y));
	const ldtk::Field<ldtk::EnumValue>& color_solution_field = puzzle_bubble_fake_entity.getField<ldtk::EnumValue>("Color");
	const COLOUR_STATE colour = color_state_from_string(color_solution_field.value().name);
	createPuzzleBubbleFake(puzzle_bubble_fake_entity.layer->level->uid, position, colour);
}

void load_puzzle_solution(const ldtk::Entity& puzzle_solution_entity)
{
	const ldtk::ArrayField<ldtk::EntityRef>& puzzle_piece_ref_fields = puzzle_solution_entity.getField<ldtk::FieldType::ArrayEntityRef>("PuzzlePieces");
	const ldtk::ArrayField<ldtk::EntityRef>& trigger_ref_fields = puzzle_solution_entity.getField<ldtk::FieldType::ArrayEntityRef>("Triggers");
	std::vector<std::string> puzzle_piece_ids;
	std::vector<std::string> puzzle_trigger_ids;

	for (const ldtk::Field<ldtk::EntityRef>& puzzle_piece_ref_field : puzzle_piece_ref_fields)
	{
		const ldtk::EntityRef puzzle_piece_ref = puzzle_piece_ref_field.value();
		puzzle_piece_ids.push_back(puzzle_piece_ref.entity_iid.str());
	}

	for (const ldtk::Field<ldtk::EntityRef>& trigger_ref_field : trigger_ref_fields)
	{
		const ldtk::EntityRef trigger_ref = trigger_ref_field.value();
		puzzle_trigger_ids.push_back(trigger_ref.entity_iid.str());
	}

	Entity puzzle_entity;
	registry.puzzles.emplace(puzzle_entity);
	registry.puzzle_build_requests.insert(Entity(), { puzzle_entity, puzzle_piece_ids, puzzle_trigger_ids });
}

void load_puzzle_wall(const ldtk::Entity& puzzle_wall_entity)
{
	std::cout << puzzle_wall_entity.getName() << std::endl;
	const ldtk::IntPoint& wall_world_position = puzzle_wall_entity.getWorldPosition();
	const ldtk::IntPoint& wall_size = puzzle_wall_entity.getSize();
	const vec2 position = scale_to_level(vec2(wall_world_position.x, wall_world_position.y));
	const vec2 size = scale_to_level(vec2(wall_size.x, wall_size.y));
	for (Entity& e : registry.unloadables.entities) {
		vec2& pos = registry.unloadables.get(e).initial_position;
		//account for roundoff error
		if (abs(pos.x - position.x) < 10 && abs(pos.y - position.y) < 10) {
			//don't load this, it was loaded
			return;
		}
	}
	const vec4 PUZZLE_WALL_COLOUR = vec4(vec3(105, 30, 105) / 255.0f, 1.0);
	Entity& wall = createWall(puzzle_wall_entity.layer->level->uid, position, size.x, size.y, PUZZLE_WALL_COLOUR);
	registry.renderRequests.get(wall).layer = RENDER_LAYER::WALLS_FRONT;
	registry.puzzle_triggers.insert(wall, { puzzle_wall_entity.iid.str(), [&]() {} });
	registry.unloadables.emplace(wall);
}
