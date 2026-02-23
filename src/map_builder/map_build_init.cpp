#include "ecs/registry.hpp"
#include "map_builder/map_build_init.hpp"
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
	const Entity& wall_rectangle_entity = createRectangle(position, width, height, color_from_state);
	add_wall_components(wall_rectangle_entity, color_state);

	return wall_rectangle_entity;
}

Entity createWall(vec2 top_left, vec2 bottom_right, COLOUR_STATE color_state)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	const Entity& wall_rectangle_entity = createRectangle(top_left, bottom_right, color_from_state);
	add_wall_components(wall_rectangle_entity, color_state);

	return wall_rectangle_entity;
}

Entity createToggle(vec2 position, float radius, COLOUR_STATE color_state, std::vector<Entity> controlled_walls)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	Entity& toggle_entity = createCircle(position, radius, color_from_state);
	Toggle& toggle = registry.toggles.emplace(toggle_entity);
	ColourState& color_state_component = registry.colourStates.emplace(toggle_entity);
	color_state_component.curr_colour_state = color_state;
	toggle.walls_to_toggle = controlled_walls;
	Collidable& col = registry.collidables.emplace(toggle_entity);
	col.inelastic = true;

	return toggle_entity;
}
