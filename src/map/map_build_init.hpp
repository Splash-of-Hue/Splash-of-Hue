#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "renderer/render_constants.hpp"

/// <summary>
/// Creates a wall and returns the entity
/// </summary>
/// <param name="position">The (x, y) coordinates of the center of the wall.</param>
/// <param name="width">The size of the wall along the x-axis.</param>
/// <param name="height">The size of the wall along the y-axis.</param>
/// <param name="color_state">The color of the wall.</param>
/// <returns>The entity of the wall.</returns>
Entity createWall(vec2 position, float width, float height, COLOUR_STATE color_state = COLOUR_STATE::NEUTRAL);

/// <summary>
/// Creates a wall and returns the entity
/// </summary>
/// <param name="top_left">The (x, y) coordinates of the top-left.</param>
/// <param name="bottom_right">The (x, y) coordinates of the bottom-right.</param>
/// <param name="color_state">The color of the wall.</param>
/// <returns>The entity of the wall.</returns>
Entity createWall(vec2 top_left, vec2 bottom_right, COLOUR_STATE color_state = COLOUR_STATE::NEUTRAL);

/// <summary>
/// Creates a toggle and returns the entity
/// </summary>
/// <param name="position">The (x, y) coordinates of the center of the toggle.</param>
/// <param name="radius">The radius of the toggle.</param>
/// <param name="color_state">The color of the toggle.</param>
/// <param name="controlled_walls">The entities of the walls controlled by the toggle.</param>
/// <returns>The entity of the toggle.</returns>
Entity createToggle(vec2 position, float radius, COLOUR_STATE color_state, std::vector<Entity> controlled_walls);

/// <summary>
/// Creates a toggle and returns the entity
/// </summary>
/// <param name="tile_id">The unique ID of the tile.</param>
/// <param name="texture_id">The texture to use to render the tile.</param>
/// <param name="tile_position">The (x, y) coordinates of the tile on the tilemap.</param>
/// <param name="dest_rect">The (x, y, width, height) rectangle of the tile in world space.</param>
/// <param name="source_rect">The (x, y, width, height) rectangle to use as the source on the texture for the tile.</param>
/// <returns>The entity of the map tile.</returns>
Entity createMapTile(int tile_id, TEXTURE_ASSET_ID texture_id, ivec2 tile_position, vec4 dest_rect, vec4 source_rect, MAP_TILE_TYPE map_tile_type = MAP_TILE_TYPE::DEFAULT);

/// <summary>
/// Creates a spawn point, that if the player steps on, they are sent somewhere else
/// </summary>
/// <param name="level_uid">The unique ID of the level that the spawn point is in.</param>
/// <param name="connected_level_uid">The unique ID of the level that the spawn point points to.</param>
/// <param name="position">The position of the spawn point in the world.</param>
/// <param name="connected_position">The place in the world to be sent to once stepped on.</param>
/// <returns>The entity of the spawn point.</returns>
Entity createSpawnPoint(const int level_uid, const int connected_level_uid, vec2 position, vec2 connected_position);
/// <summary>
/// Creates dungeon wall that players cannot pass
/// </summary>
/// <returns>The entity of the wall.</returns>
Entity createDungeonWall(vec2 position, float width, float height);