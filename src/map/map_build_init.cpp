#include "creature_system/creature_constants.hpp"
#include "ecs/registry.hpp"
#include "map/map_build_init.hpp"
#include "map_components.hpp"
#include "renderer/render_constants.hpp"
#include "world_init.hpp"
#include <string>

Entity createMossWall(std::string id, vec2 position, vec2 size, COLOUR_STATE color_state)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	const Entity& moss_wall_entity = createRectangle(position, size.x, size.y, RENDER_LAYER::WALLS_FRONT, color_from_state);
	Collidable& col = registry.collidables.emplace(moss_wall_entity);
	col.inelastic = true;
	registry.colouredWall.insert(moss_wall_entity, { id });
	registry.colourStates.insert(moss_wall_entity, { color_state, color_state });

	return moss_wall_entity;
}

Entity createMapTile(int tile_id, TEXTURE_ASSET_ID texture_id, ivec2 tile_position, vec4 dest_rect, vec4 source_rect, MAP_TILE_TYPE map_tile_type)
{
	Entity map_tile_entity;
	MapTile map_tile = { tile_id, tile_position, map_tile_type };
	registry.mapTiles.insert(map_tile_entity, map_tile);
	registry.positions.insert(map_tile_entity, { dest_rect.x, dest_rect.y });
	registry.scales.insert(map_tile_entity, { dest_rect.p, dest_rect.q });
	registry.angles.insert(map_tile_entity, { 0 });
	registry.renderSourceRects.insert(map_tile_entity, { vec2(source_rect.x, source_rect.y), vec2(source_rect.p, source_rect.q) });
	registry.textured.insert(map_tile_entity, { texture_id });
	registry.colors.insert(map_tile_entity, { DEFAULT_COLOUR_VEC4 });
	registry.renderRequests.insert(
		map_tile_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR
		}
	);

	return map_tile_entity;
}

Entity createTeleporter(const int level_uid, const int connected_level_id, vec2 position, vec2 connected_position, DIRECTION offset_direction)
{
	Entity teleporter_entity = createCircle(position, 10.0f, RENDER_LAYER::FLOOR_INTERACTABLES, { DEFAULT_COLOUR_VEC4 });
	Teleporter teleporter = { level_uid, connected_level_id, offset_direction, vec2(connected_position.x, connected_position.y) };
	registry.teleporters.emplace(teleporter_entity, teleporter);
	registry.collidables.emplace(teleporter_entity);
	registry.levelEntities.insert(teleporter_entity, { level_uid });

	return teleporter_entity;
}
