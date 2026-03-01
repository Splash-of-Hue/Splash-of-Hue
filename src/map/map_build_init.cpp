#include "ecs/registry.hpp"
#include "map/map_build_init.hpp"
#include "map_components.hpp"
#include "world_init.hpp"

static void add_wall_components(Entity entity, COLOUR_STATE color_state)
{
	Collidable& col = registry.collidables.emplace(entity);
	col.inelastic = true;
	registry.colouredWall.emplace(entity);
	ColourState& csCW = registry.colourStates.emplace(entity);
	csCW.curr_colour_state = color_state;
	csCW.prev_colour_state = color_state;
}

Entity createWall(vec2 position, float width, float height, COLOUR_STATE color_state)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	const Entity& wall_rectangle_entity = createRectangle(position, width, height, RENDER_LAYER::WALLS_BACK, color_from_state);
	add_wall_components(wall_rectangle_entity, color_state);

	return wall_rectangle_entity;
}

Entity createWall(vec2 top_left, vec2 bottom_right, COLOUR_STATE color_state)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	const Entity& wall_rectangle_entity = createRectangle(top_left, bottom_right, RENDER_LAYER::WALLS_BACK, color_from_state);
	add_wall_components(wall_rectangle_entity, color_state);

	return wall_rectangle_entity;
}

Entity createToggle(vec2 position, float radius, COLOUR_STATE color_state, std::vector<Entity> controlled_walls)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	Entity& toggle_entity = createCircle(position, radius, RENDER_LAYER::FLOOR_INTERACTABLES, color_from_state);
	Toggle& toggle = registry.toggles.emplace(toggle_entity);
	ColourState& color_state_component = registry.colourStates.emplace(toggle_entity);
	color_state_component.curr_colour_state = color_state;
	toggle.walls_to_toggle = controlled_walls;
	Collidable& col = registry.collidables.emplace(toggle_entity);
	col.inelastic = true;

	return toggle_entity;
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

Entity createSpawnPoint(const int level_uid, const int connected_level_id, vec2 position, vec2 connected_position)
{
	Entity spawn_point_entity;
	Size size = { SPAWN_POINT_RADIUS, SPAWN_POINT_RADIUS };
	registry.sizes.insert(spawn_point_entity, size);
	registry.positions.insert(spawn_point_entity, position);
	SpawnPoint spawn_point = { level_uid, connected_level_id, vec2(connected_position.x, connected_position.y) };
	registry.spawnPoints.emplace(spawn_point_entity, spawn_point);
	registry.collidables.emplace(spawn_point_entity);

	return spawn_point_entity;
}
