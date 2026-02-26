#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "creature_system/creature_constants.hpp"
#include "renderer/render_constants.hpp"

void addGeneralCreatureComponents(
	Entity creature, 
	vec2 position, 
	float width, 
	float height, 
	COLOUR_STATE color_state, 
	CREATURETYPE creature_type, 
	TEXTURE_ASSET_ID texture_asset_id
);

Entity createAnglerfish(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createCrab(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createMinnows(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createSawfish(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createShrimp(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createWhale(vec2 position, float width, float height, COLOUR_STATE color_state);
