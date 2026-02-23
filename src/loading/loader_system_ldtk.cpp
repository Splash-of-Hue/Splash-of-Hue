#include "common.hpp"
#include "ecs/entity.hpp"
#include "loader_system.hpp"
#include "loading_constants.hpp"
#include "map_builder/map_build_init.hpp"
#include <iostream>
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Project.hpp>
#include <LDtkLoader/Tileset.hpp>
#include <map_builder/map_build_init.hpp>
#include <renderer/render_constants.hpp>
#include <string>

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

void load_entity_layer(const ldtk::Layer& layer)
{
	// iterate on Enemy entities
	for (const auto& layer_entity : layer.allEntities())
	{
		std::cout << "Layer " << layer.getName() << ": Entity " << layer_entity.getName() << " exists." << std::endl;
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
