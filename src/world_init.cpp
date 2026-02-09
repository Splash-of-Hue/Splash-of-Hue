#include "world_init.hpp"
#include "ecs/registry.hpp"
#include <iostream>

Entity createGridLine(vec2 start_pos, vec2 end_pos)
{
	Entity entity = Entity();

	registry.gridLines.insert(entity, { start_pos, end_pos });

	registry.renderRequests.insert(
		entity,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::GRADIENT_CIRCLE
		}
	);
	
	registry.colors.emplace(entity, vec4(1.0f, 0.0f, 0.0f, 1.0f));

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{
			EFFECT_ASSET_ID::EFFECT_COUNT,
			GEOMETRY_BUFFER_ID::RECTANGLE
		}
	);
	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createRectangle(vec2 position, float width, float height, vec4 color)
{
	Entity entity = Entity();

	Size size = { width, height };
	registry.positions.insert(entity, position);
	registry.sizes.insert(entity, size);
	registry.rectangles.emplace(entity);
	registry.colors.emplace(entity, color);
	
	registry.renderRequests.insert(
		entity,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::RECTANGLE
		}
	);

	return entity;
}

Entity createRectangle(vec2 top_left, vec2 bottom_right, vec4 color)
{
	Entity entity = Entity();

	auto rectangle_diagonal = (bottom_right - top_left);
	Size size = { rectangle_diagonal.x, rectangle_diagonal.y };
	auto center = (bottom_right + top_left) * vec2(0.5);
	registry.positions.insert(entity, center);
	registry.sizes.insert(entity, size);
	registry.rectangles.emplace(entity);
	registry.colors.emplace(entity, color);

	registry.renderRequests.insert(
		entity,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::RECTANGLE
		}
	);

	return entity;
}

Entity createCircle(vec2 position, float radius, vec4 color)
{
	Entity entity = Entity();

	Size size = { radius, radius };
	registry.positions.insert(entity, position);
	registry.sizes.insert(entity, size);
	registry.circles.emplace(entity);
	registry.colors.emplace(entity, color);
	
	Collidable& col = registry.collidables.emplace(entity);
	col.overlay = true;
	registry.renderRequests.insert(
		entity,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::CIRCLE
		}
	);
	
	return entity;
}

Entity createPlayer(vec2 position, float width, float height, vec4 color)
{
	Entity player = Entity();
	registry.positions.insert(player, position);
	registry.scales.insert(player, vec2((float)2.0f*GRID_CELL_WIDTH_PX, (float)2.0f*GRID_CELL_HEIGHT_PX));
	registry.angles.emplace(player);
	registry.angles.get(player).degrees = 0.0f;
	registry.sizes.insert(player, Size{ width, height });
	registry.colors.emplace(player, color);

	Collidable& col = registry.collidables.emplace(player);
	col.overlay = true;

	registry.textured.emplace(player);
	registry.textured.get(player).used_texture = TEXTURE_ASSET_ID::PLAYER_TEXTURE_01;
	
	registry.renderRequests.insert(
		player,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	return player;
}


Entity createSawfish(vec2 position, float width, float height, vec4 color)
{
	Entity sawfish = Entity();
	registry.positions.insert(sawfish, position);
	registry.scales.insert(sawfish, vec2((float)2.0f * GRID_CELL_WIDTH_PX, (float)2.0f * GRID_CELL_HEIGHT_PX));
	registry.angles.emplace(sawfish);
	registry.angles.get(sawfish).degrees = 0.0f;
	registry.sizes.insert(sawfish, Size{ width, height });
	registry.colors.emplace(sawfish, color);

	Collidable& col = registry.collidables.emplace(sawfish);
	col.overlay = true;

	registry.textured.emplace(sawfish);
	registry.textured.get(sawfish).used_texture = TEXTURE_ASSET_ID::SAWFISH_01;

	registry.renderRequests.insert(
		sawfish,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	return sawfish;
}


