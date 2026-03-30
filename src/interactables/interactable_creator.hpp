#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "interactables/interactable_constants.hpp"
#include "interactables/interactable_components.hpp"
#include <string>
#include <vector>

Entity addGeneralInteractableComponents(vec2 position, Size& size, INTERACTABLE_TYPE interactable_type);

Entity createBox(vec2 position, float width, float height);
Entity createColourOrb(vec2 position, float radius, COLOUR_STATE colour_state);
Entity createMushroom(vec2 position, float radius, COLOUR_STATE colour_state, MUSHROOM_SIZE mushroom_size);

/// <summary>
/// Creates a toggle and returns the entity
/// </summary>
/// <param name="level_uid">The unique ID of the level that the moss toggle is in.</param>
/// <param name="position">The (x, y) coordinates of the center of the toggle.</param>
/// <param name="size">The (width, height) of the toggle.</param>
/// <param name="connected_wall_ids">The IDs of the walls controlled by the toggle.</param>
/// <returns>The entity of the toggle.</returns>
Entity createMossToggle(const int level_uid, vec2 position, vec2 size, std::vector<std::string> connected_wall_ids);

/// <summary>
/// Creates a pressure plate and returns the entity. 
/// Acts similarly to the moss toggle in the sense that it controls doors, 
/// except the pressure plate must be stepped on by a player/creature
/// </summary>
/// <param name="level_uid">The unique ID of the level that the pressure plate is in.</param>
/// <param name="position">The (x, y) coordinates of the center of the pressure plate.</param>
/// <param name="size">The (width, height) of the pressure plate.</param>
/// <param name="connected_wall_ids">The IDs of the walls controlled by the pressure plate.</param>
/// <returns>The entity of the pressure plate.</returns>
Entity createPressurePlate(const int level_uid, vec2 position, vec2 size, std::vector<std::string> connected_wall_ids);

Entity createPuzzleBubble(const int level_uid, std::string id, vec2 position, COLOUR_STATE colour_solution);
Entity createPuzzleBubbleFake(const int level_uid, vec2 position, COLOUR_STATE colour);

Entity createFloorPictureControls(vec2 position, float width, float height, CONTROL_PNG_TYPE control_png_type);
Entity createFloorPictureHint(vec2 position, float width, float height, HINT_PNG_TYPE hint_png_type);
