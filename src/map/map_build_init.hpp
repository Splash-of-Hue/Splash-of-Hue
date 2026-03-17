#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "renderer/render_constants.hpp"
#include <string>

/// <summary>
/// Creates a wall and returns the entity
/// </summary>
/// <param name="level_uid">The unique ID of the level that the teleporter is in.</param>
/// <param name="id">The unique ID of the moss wall.</param>
/// <param name="position">The (x, y) coordinates of the center of the wall.</param>
/// <param name="size">The (width, height) of the wall.</param>
/// <param name="color_state">The color of the wall.</param>
/// <returns>The entity of the wall.</returns>
Entity createMossWall(const int level_uid, std::string id, vec2 position, vec2 size, COLOUR_STATE color_state);

/// <summary>
/// Creates a toggle and returns the entity
/// </summary>
/// <param name="tile_id">The unique ID of the tile.</param>
/// <param name="texture_id">The texture to use to render the tile.</param>
/// <param name="tile_position">The (x, y) coordinates of the tile on the tilemap.</param>
/// <param name="dest_rect">The (x, y, width, height) rectangle of the tile in world space.</param>
/// <param name="source_rect">The (x, y, width, height) rectangle to use as the source on the texture for the tile.</param>
/// <returns>The entity of the map tile.</returns>
Entity createMapTile(int tile_id, TEXTURE_ASSET_ID texture_id, ivec2 tile_position, vec4 dest_rect, vec4 source_rect, TILE_TYPE tile_type = TILE_TYPE::DEFAULT);

/// <summary>
/// Creates a teleporter, that if the player steps on, they are sent somewhere else
/// </summary>
/// <param name="level_uid">The unique ID of the level that the teleporter is in.</param>
/// <param name="connected_level_uid">The unique ID of the level that the teleporter points to.</param>
/// <param name="position">The position of the teleporter in the world.</param>
/// <param name="size">The (width, height) of the teleporter.</param>
/// <param name="connected_position">The place in the world to be sent to once stepped on.</param>
/// <param name="offset_direction">The direction whatever teleports in will be offset to.</param>
/// <returns>The entity of the teleporter.</returns>
Entity createTeleporter(const int level_uid, const int connected_level_uid, vec2 position, vec2 size, vec2 connected_position, DIRECTION offset_direction);

/// <summary>
/// Creates dungeon wall that players cannot pass
/// </summary>
/// <returns>The entity of the wall.</returns>
Entity createWall(const int level_uid, vec2 position, float width, float height);
