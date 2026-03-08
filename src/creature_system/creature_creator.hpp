#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "creature_system/creature_constants.hpp"
#include "creature_system/creature_components.hpp"
#include "renderer/render_constants.hpp"

void addGeneralCreatureComponents(
	Entity creature, 
	vec2 position, 
	float width, 
	float height, 
	COLOUR_STATE color_state, 
	CREATURETYPE creature_type, 
	TEXTURE_ASSET_ID texture_asset_id,
	bool shouldHaveInelasticCollision = true
);

Entity createAnglerfish(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createCrab(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createMinnows(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createSawfish(vec2 position, float width, float height, COLOUR_STATE color_state);
Entity createShrimp(const int id, vec2 position, float width, float height, COLOUR_STATE color_state, DIRECTION curr_direction_facing = DIRECTION::UP, int num_arms = 2);
Entity createWhale(vec2 position, float width, float height, COLOUR_STATE color_state);

// Helpers
Entity createLeftArm(vec2 shrimp_position, float shrimp_width, vec4 colour);
Entity createRightArm(vec2 shrimp_position, float shrimp_width, vec4 colour);
Entity createUpArm(vec2 shrimp_position, float shrimp_width, vec4 colour);
Entity createDownArm(vec2 shrimp_position, float shrimp_width, vec4 colour);

// TODO 0aEQcx13
//void createThreeArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height);
void createTwoArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height);
void createOneArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height);
