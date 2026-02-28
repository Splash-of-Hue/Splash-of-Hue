#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "interactables/interactable_constants.hpp"
#include "renderer/render_constants.hpp"

Entity addGeneralInteractableComponents(vec2 position, Size& size, INTERACTABLE_TYPE interactable_type);

Entity createBox(vec2 position, float width, float height);
Entity createColourOrb(vec2 position, float radius, COLOUR_STATE colour_state);
Entity createMushroom(vec2 position, float radius, COLOUR_STATE colour_state);
