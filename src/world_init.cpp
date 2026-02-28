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
	registry.renderRequests.insert(
		entity,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::CIRCLE
		}
	);
	
	return entity;
}

ConvexPolygon createPolygon(const Entity& e) {
	
	//if it already has a polygon, its position needs to be updated
	if (registry.polygons.has(e)) {
		ConvexPolygon& polygon = registry.polygons.get(e);
		vec2& pos = registry.positions.get(e);
		vec2 change_in_pos = pos - polygon.centre;
		for (vec2& v : polygon.vertices) {
			v += change_in_pos;
		}
		polygon.centre = pos;
		return polygon;
	}
	//default is rectangle
	ConvexPolygon& polygon = registry.polygons.emplace(e);
	vec2& pos = registry.positions.get(e);
	Size& size = registry.sizes.get(e);
	polygon.centre = pos;
	//Entity heights are too large, this is a temp fix
	//TODO: DMdDb4NW
	polygon.vertices.push_back(vec2(pos.x - size.width * 0.5f, pos.y - size.height * 0.15f)); // bottom left
	polygon.vertices.push_back(vec2(pos.x + size.width * 0.5f, pos.y - size.height * 0.15f)); // bottom right
	polygon.vertices.push_back(vec2(pos.x + size.width * 0.5f, pos.y + size.height * 0.15f)); // top right
	polygon.vertices.push_back(vec2(pos.x - size.width * 0.5f, pos.y + size.height * 0.15f)); // top left
	
	return polygon;
}
