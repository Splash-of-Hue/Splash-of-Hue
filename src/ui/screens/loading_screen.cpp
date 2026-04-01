#pragma once
#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "game_state/game_state_constants.hpp"
#include "map/map_components.hpp"
#include "loading_screen.hpp"
#include "shapes/shape_build_init.hpp"
#include "text/text_constants.hpp"
#include "text/text_init.hpp"
#include "ui/ui_components.hpp"
#include "ui/ui_constants.hpp"
#include <functional>
#include <vector>

void LoadingScreen::enable()
{
	Entity& backdrop_entity = create_backdrop();
	// Add to a list to keep track of what should be removed when disabled
	ui_entities.push_back(backdrop_entity);

	// For each corner, create the pretty outline
	for (int i = 0; i < 4; i++)
	{
		ui_entities.push_back(create_ui_corner(i));
	}

	Entity& loading_text = create_text("Loading...", LOADING_TEXT_POSITION, vec2(1), vec4(1), FONT_ID::BEAUTIDUL_HEARTBEAT);
	ui_entities.push_back(loading_text);

	// Request state change to trigger
	registry.game_state_change_requests.insert(Entity(), { GAME_STATE::LOADING });
	active = true;
}

void LoadingScreen::disable()
{
	for (Entity& entity : ui_entities)
	{
		registry.remove_all_components_of(entity);
	}

	ui_entities.clear();
	registry.game_state_change_requests.insert(Entity(), { GAME_STATE::PLAYING });
	active = false;
}

Entity LoadingScreen::create_backdrop()
{
	Entity& backdrop_entity = createRectangle(vec2(0), vec2(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), RENDER_LAYER::UI_BACKGROUND, BLACK_COLOUR_VEC4);
	// Add to a list to destroy when disabled
	registry.renderRequests.get(backdrop_entity).layer = RENDER_LAYER::UI_BACKGROUND;
	registry.ignoredByCamera.emplace(backdrop_entity);

	return backdrop_entity;
}

// Index: 0 = top left, 1 = top right, 2 = bottom right, 3 = bottom left
Entity LoadingScreen::create_ui_corner(int corner_index)
{
	Entity ui_corner_entity;
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	const vec2 manual_offset = vec2(50, 20);
	const float corner_size = 50.0f;
	// Flip the angle on index 2 and 3
	float angle = corner_index < 2 ? 0 : 180;
	// Mirror on the x-axis for index 1 and 3
	float x_reflection = (corner_index % 2) == 0 ? 1 : -1;
	registry.angles.insert(ui_corner_entity, { angle });
	registry.scales.insert(ui_corner_entity, vec2(x_reflection * corner_size, corner_size));
	registry.sizes.insert(ui_corner_entity, { corner_size, corner_size });
	// Position is based on loading text
	vec2 corner_position = vec2(LOADING_TEXT_POSITION.x, WINDOW_HEIGHT_PX - LOADING_TEXT_POSITION.y) - manual_offset;
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	// The x position is shared between 0/3 and 1/2
	corner_position.x += (corner_index % 3) == 0 ? 0 : /* For now add manual width */ 200.0f;
	registry.positions.insert(ui_corner_entity, corner_position);
	// The rest is normal
	registry.ignoredByCamera.emplace(ui_corner_entity);
	registry.textured.insert(ui_corner_entity, { TEXTURE_ASSET_ID::UI_CORNER_DECORATION });
	registry.renderRequests.insert(
		ui_corner_entity,
		{
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::UI,
		});

	return ui_corner_entity;
}
