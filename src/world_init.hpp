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
Entity createRectangle(vec2 position, float width, float height, vec3 color = {1.0, 1.0, 1.0});

/// <summary>
/// Creates an entity associated with a rectangle and returns the entity
/// Should have Rectangle component when returned.
/// </summary>
/// <param name="top_left">The (x, y) coordinates of the top-left.</param>
/// <param name="bottom_right">The (x, y) coordinates of the bottom-right.</param>
/// <param name="color">The color of the rectangle, defaults to white if not set.</param>
/// <returns></returns>
Entity createRectangle(vec2 top_left, vec2 bottom_right, vec3 color = { 1.0, 1.0, 1.0 });
