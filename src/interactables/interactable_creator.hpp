#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "interactables/interactable_constants.hpp"
#include <string>
#include <vector>

Entity addGeneralInteractableComponents(vec2 position, Size& size, INTERACTABLE_TYPE interactable_type);

Entity createBox(vec2 position, float width, float height);
Entity createColourOrb(vec2 position, float radius, COLOUR_STATE colour_state);
Entity createFloorPictureControls(vec2 position, float width, float height, CONTROL_PNG_TYPE control_png_type);
Entity createMushroom(vec2 position, float radius, COLOUR_STATE colour_state);

/// <summary>
/// Creates a toggle and returns the entity
/// </summary>
/// <param name="level_uid">The unique ID of the level that the moss toggle is in.</param>
/// <param name="position">The (x, y) coordinates of the center of the toggle.</param>
/// <param name="size">The (width, height) of the toggle.</param>
/// <param name="color_state">The color of the toggle.</param>
/// <param name="connected_wall_ids">The IDs of the walls controlled by the toggle.</param>
/// <returns>The entity of the toggle.</returns>
Entity createMossToggle(const int level_uid, vec2 position, vec2 size, COLOUR_STATE color_state, std::vector<std::string> connected_wall_ids);
