#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "renderer/render_system.hpp"

// grid lines to show tile positions
Entity createGridLine(vec2 start_pos, vec2 end_pos);

// debugging red lines
Entity createLine(vec2 position, vec2 size);

/// <summary>
/// Creates an entity associated with a rectangle and returns the entity
/// Should have Rectangle component when returned.
/// </summary>
/// <param name="position">The (x, y) coordinates of the center of the rectangle.</param>
/// <param name="width">The size of the rectangle along the x-axis.</param>
/// <param name="height">The size of the rectangle along the y-axis.</param>
/// <param name="color">The color of the rectangle, defaults to white if not set.</param>
/// <returns></returns>
Entity createRectangle(vec2 position, float width, float height, vec4 color = { 1.0, 1.0, 1.0, 1.0 });

/// <summary>
/// Creates an entity associated with a rectangle and returns the entity
/// Should have Rectangle component when returned.
/// </summary>
/// <param name="top_left">The (x, y) coordinates of the top-left.</param>
/// <param name="bottom_right">The (x, y) coordinates of the bottom-right.</param>
/// <param name="color">The color of the rectangle, defaults to white if not set.</param>
/// <returns></returns>
Entity createRectangle(vec2 top_left, vec2 bottom_right, vec4 color = { 1.0, 1.0, 1.0, 1.0 });

/// <summary>
/// Creates an entity associated with a circle and returns the entity
/// Should have Circle component when returned.
/// </summary>
/// <param name="position">The (x, y) coordinates of the center of the circle.</param>
/// <param name="radius">The size of the circle's radius.</param>
/// <param name="color">The color of the circle, defaults to white if not set.</param>
/// <returns></returns>
Entity createCircle(vec2 position, float radius, vec4 color = { 1.0, 1.0, 1.0, 1.0});

/// <summary>
/// Creates an entity associated with a sawfish sprite and returns the entity
/// </summary>
/// <param name="position">The (x, y) coordinates of the center of the sawfish.</param>
/// <param name="width">The width of the sawfish.</param>
/// <param name="height">The height of the sawfish.</param>
/// <param name="color">The color of the sawfish, defaults to white if not set.</param>
/// <returns></returns>

/// TODO: 69 - move to creature creator when that is made
Entity createSawfish(vec2 position, float width, float height, vec4 color);
