#include "loader_system.hpp"
#include "loading_constants.hpp"
#include <iostream>
#include <LDtkLoader/DataTypes.hpp>
#include <LDtkLoader/Project.hpp>
#include <string>

static bool project_loaded = false;
static ldtk::Project ldtk_project;

void load_tile_layer(const ldtk::Layer& layer)
{
	// iterate on the tiles of the layer
	for (const auto& tile : layer.allTiles())
	{
		std::cout << "Layer " << layer.getName() << ": Tile with ID " << tile.tileId << " at coordinate ID " << tile.coordId << std::endl;
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
	// Do nothing, this will be implemented later.
	// Later we will want to remove things from memory and registry.
}
