#include "common.hpp"
#include "ecs/entity.hpp"
#include "loader_system.hpp"
#include "loading_constants.hpp"
#include "map_builder/map_build_init.hpp"
#include <creature_behaviour_system/creature_constants.hpp>
#include <iostream>
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Enum.hpp>
#include <LDtkLoader/Field.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tileset.hpp>
#include <renderer/render_constants.hpp>
#include <string>
#include <world_init.hpp>

static bool project_loaded = false;
static ldtk::Project ldtk_project;

void load_tile_layer(const ldtk::Layer& layer)
{
	const ldtk::IntPoint& level_size = layer.level->size;
	const ldtk::Tileset& layer_tileset = layer.getTileset();
	const ldtk::IntPoint& tileset_texture_size = layer_tileset.texture_size;
	const int& tile_size = layer_tileset.tile_size;
	const float level_scale_x = WINDOW_WIDTH_PX / (float)level_size.x;
	const float level_scale_y = WINDOW_HEIGHT_PX / (float)level_size.y;

	// iterate on the tiles of the layer
	for (const auto& tile : layer.allTiles())
	{
		const ldtk::IntPoint& tile_position = tile.getPosition();
		const ldtk::IntPoint& tile_world_position = tile.getWorldPosition();
		const ldtk::IntRect& tile_texture_rect = tile.getTextureRect();
		vec4 destination_rect = {
			(tile_world_position.x + tile_size / 2) * level_scale_x,
			(tile_world_position.y + tile_size / 2) * level_scale_y,
			tile_size * level_scale_x,
			tile_size * level_scale_y
		};
		vec4 source_rect = {
			tile_texture_rect.x / (float)tileset_texture_size.x,
			tile_texture_rect.y / (float)tileset_texture_size.y,
			tile_size / (float)tileset_texture_size.x,
			tile_size / (float)tileset_texture_size.y
		};

		// TODO mgydr9IJ: Get texture ID dynamically instead of always using ROOMS
		Entity& tile_entity = createMapTile(tile.tileId, TEXTURE_ASSET_ID::ROOMS, ivec2(tile_position.x, tile_position.y), destination_rect, source_rect);
	}
}

void load_creatures(const ldtk::Layer& layer)
{
	const ldtk::IntPoint& level_size = layer.level->size;
	const float level_scale_x = WINDOW_WIDTH_PX / (float)level_size.x;
	const float level_scale_y = WINDOW_HEIGHT_PX / (float)level_size.y;

	// iterate on Enemy entities
	for (const ldtk::Entity& creature_entity : layer.allEntities())
	{
		const ldtk::Field<ldtk::EnumValue>& creature_enum_type = creature_entity.getField<ldtk::EnumValue>("CreatureType");
		const CREATURETYPE& creature_type = creature_type_from_string(creature_enum_type.value().name);
		const ldtk::Field<ldtk::EnumValue>& color_state_enum_type = creature_entity.getField<ldtk::EnumValue>("ColorState");
		const COLOUR_STATE& color_state = color_state_from_string(color_state_enum_type.value().name);
		const ldtk::IntPoint& creature_world_position = creature_entity.getWorldPosition();
		const ldtk::IntPoint& creature_size = creature_entity.getSize();
		const vec2 position = vec2(creature_world_position.x * level_scale_x, creature_world_position.y * level_scale_y);

		switch (creature_type)
		{
			case CREATURETYPE::SAWFISH:
				createSawfish(position, creature_size.x * level_scale_x, creature_size.y * level_scale_y, color_state);
		}
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
		case LOADING_LAYER_TYPE::INVALID:
			std::cout << "Layer " << layer.getName() << ": Invalid layer type." << std::endl;
			return;
	}
}

void LoaderSystem::load_level(int level_uid)
{
	if (!project_loaded)
	{
		// load the file
		ldtk_project.loadFromFile(LDTK_PROJECT_PATH);
	}

	// get a world
	const auto& world_id = ldtk::IID::IID(WORLD_ID_STRING);
	const auto& level_id = ldtk::IID::IID(LEVEL_ID_STRING);
	const auto& world = ldtk_project.getWorld(world_id);

	// get a level
	const auto& level = world.getLevel(level_id);

	// get a layer
	const auto& level_layers = level.allLayers();

	for (const auto& layer : level_layers)
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
}

void LoaderSystem::unload_level(int level_uid)
{
	// TODO Mh0WEPiT: Do nothing, this will be implemented later.
	// Later we will want to remove things from memory and registry.
}
