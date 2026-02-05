#include "world_init.hpp"
#include "ecs/registry.hpp"
#include <iostream>

Entity createGridLine(vec2 start_pos, vec2 end_pos)
{
	Entity entity = Entity();

	registry.gridLines.insert(entity, { start_pos, end_pos });

	// re-use the "DEBUG_LINE" renderRequest
	
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::GRADIENT_CIRCLE
		}
	);
	

	// TODO A1: grid line color (choose your own color)
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
			// usage TEXTURE_COUNT when no texture is needed, i.e., an .obj or other vertices are used instead
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::EFFECT_COUNT,
			GEOMETRY_BUFFER_ID::RECTANGLE
		}
	);

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createRectangle(vec2 position, float width, float height, vec3 color)
{
	Entity entity = Entity();

	Size size = { width, height };
	registry.positions.insert(entity, position);
	registry.sizes.insert(entity, size);
	registry.rectangles.emplace(entity);
	registry.colors.emplace(entity, color);
	registry.inelasticCollisions.emplace(entity); // TODO: inelastic for rectangle, overlay for circle
	
	registry.renderRequests.insert(
		entity,
		{
			// usage TEXTURE_COUNT when no texture is needed, i.e., an .obj or other vertices are used instead
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::RECTANGLE
		}
	);

	return entity;
}

Entity createRectangle(vec2 top_left, vec2 bottom_right, vec3 color)
{
	Entity entity = Entity();

	auto rectangle_diagonal = (bottom_right - top_left);
	Size size = { rectangle_diagonal.x, rectangle_diagonal.y };
	//auto center = top_left + (rectangle_diagonal * vec2(0.5));
	auto center = (bottom_right + top_left) * vec2(0.5);
	registry.positions.insert(entity, center);
	registry.sizes.insert(entity, size);
	registry.rectangles.emplace(entity);
	registry.colors.emplace(entity, color);
	registry.inelasticCollisions.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{
			// usage TEXTURE_COUNT when no texture is needed, i.e., an .obj or other vertices are used instead
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::RECTANGLE
		}
	);

	return entity;
}

Entity createCircle(vec2 position, float radius, vec3 color)
{
	Entity entity = Entity();

	Size size = { radius, radius };
	registry.positions.insert(entity, position);
	registry.sizes.insert(entity, size);
	registry.circles.emplace(entity);
	registry.colors.emplace(entity, color);

	registry.overlayCollisions.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			// usage TEXTURE_COUNT when no texture is needed, i.e., an .obj or other vertices are used instead
			TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::CIRCLE
		}
	);

	return entity;
}

