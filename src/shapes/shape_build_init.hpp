#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "renderer/render_system.hpp"

// debugging red lines
Entity createLine(vec2 position, vec2 size);

/// <summary>
/// Creates an entity associated with a rectangle and returns the entity
/// Should have Rectangle component when returned.
/// </summary>
/// <param name="position">The (x, y) coordinates of the center of the rectangle.</param>
/// <param name="width">The size of the rectangle along the x-axis.</param>
/// <param name="height">The size of the rectangle along the y-axis.</param>
/// <param name="render_layer">The layer to render the rectangle on.</param>
/// <param name="color">The color of the rectangle, defaults to white if not set.</param>
/// <returns></returns>
Entity createRectangle(vec2 position, float width, float height, RENDER_LAYER render_layer, vec4 color = DEFAULT_COLOUR_VEC4);

/// <summary>
/// Creates an entity associated with a rectangle and returns the entity
/// Should have Rectangle component when returned.
/// </summary>
/// <param name="top_left">The (x, y) coordinates of the top-left.</param>
/// <param name="bottom_right">The (x, y) coordinates of the bottom-right.</param>
/// <param name="render_layer">The layer to render the rectangle on.</param>
/// <param name="color">The color of the rectangle, defaults to white if not set.</param>
/// <returns></returns>
Entity createRectangle(vec2 top_left, vec2 bottom_right, RENDER_LAYER render_layer, vec4 color = DEFAULT_COLOUR_VEC4);
/// <summary>
/// Creates an entity associated with a circle and returns the entity
/// Should have Circle component when returned.
/// </summary>
/// <param name="position">The (x, y) coordinates of the center of the circle.</param>
/// <param name="radius">The size of the circle's radius.</param>
/// <param name="render_layer">The layer to render the circle on.</param>
/// <param name="color">The color of the circle, defaults to white if not set.</param>
/// <returns></returns>
Entity createCircle(vec2 position, float radius, RENDER_LAYER render_layer, vec4 color = DEFAULT_COLOUR_VEC4);

PolygonMesh createCircleHitbox(vec2 centre, float radius, int segments);
