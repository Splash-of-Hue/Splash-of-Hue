#include "ecs/registry.hpp"
#include "map_builder/map_build_init.hpp"
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
	const auto& wall_rectangle_entity = createRectangle(position, width, height, color_from_state);
	add_wall_components(wall_rectangle_entity, color_state);

	return wall_rectangle_entity;
}

Entity createWall(vec2 top_left, vec2 bottom_right, COLOUR_STATE color_state)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	const auto& wall_rectangle_entity = createRectangle(top_left, bottom_right, color_from_state);
	add_wall_components(wall_rectangle_entity, color_state);

	return wall_rectangle_entity;
}

Entity createToggle(vec2 position, float radius, COLOUR_STATE color_state, std::vector<Entity> controlled_walls)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	auto& toggle_entity = createCircle(position, radius, color_from_state);
	auto& toggle = registry.toggles.emplace(toggle_entity);
	auto& color_state_component = registry.colourStates.emplace(toggle_entity);
	color_state_component.curr_colour_state = color_state;
	toggle.walls_to_toggle = controlled_walls;
	Collidable& col = registry.collidables.emplace(toggle_entity);
	col.inelastic = true;

	return toggle_entity;
}

Entity createMapTile(int tile_id, TEXTURE_ASSET_ID texture_id, ivec2 tile_position, vec4 dest_rect, vec4 source_rect)
{
	Entity map_tile_entity;
	MapTile map_tile = { tile_id, tile_position };
	registry.positions.insert(map_tile_entity, { dest_rect.x, dest_rect.y });
	registry.scales.insert(map_tile_entity, { dest_rect.p, dest_rect.q });
	registry.angles.insert(map_tile_entity, { 0 });
	registry.renderSourceRects.insert(map_tile_entity, { vec2(source_rect.x, source_rect.y), vec2(source_rect.p, source_rect.q) });
	registry.textured.insert(map_tile_entity, { texture_id });
	registry.renderRequests.insert(
		map_tile_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);

	return map_tile_entity;
}
