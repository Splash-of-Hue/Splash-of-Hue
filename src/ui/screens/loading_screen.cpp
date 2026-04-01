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
	menu_entities.push_back(backdrop_entity);

	// For each corner, create the pretty outline
	for (int i = 0; i < 4; i++)
	{
		menu_entities.push_back(create_menu_corner(i));
	}

	Entity& loading_text = create_text("Loading...", LOADING_TEXT_POSITION, vec2(1), vec4(1), FONT_ID::BEAUTIDUL_HEARTBEAT);
	menu_entities.push_back(loading_text);

	// Request state change to trigger
	registry.game_state_change_requests.insert(Entity(), { GAME_STATE::LOADING });
	active = true;
}

void LoadingScreen::disable()
{
	for (Entity& entity : menu_entities)
	{
		registry.remove_all_components_of(entity);
	}

	menu_option_entities.clear();
	menu_entities.clear();
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
Entity LoadingScreen::create_menu_corner(int corner_index)
{
	Entity menu_corner_entity;
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	const vec2 manual_offset = vec2(50, 20);
	const float corner_size = 50.0f;
	// Flip the angle on index 2 and 3
	float angle = corner_index < 2 ? 0 : 180;
	// Mirror on the x-axis for index 1 and 3
	float x_reflection = (corner_index % 2) == 0 ? 1 : -1;
	registry.angles.insert(menu_corner_entity, { angle });
	registry.scales.insert(menu_corner_entity, vec2(x_reflection * corner_size, corner_size));
	registry.sizes.insert(menu_corner_entity, { corner_size, corner_size });
	// Position is based on menu options which is screen space, so use world space equivalent
	vec2 corner_position = vec2(LOADING_TEXT_POSITION.x, WINDOW_HEIGHT_PX - LOADING_TEXT_POSITION.y) - manual_offset;
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	// The x position is shared between 0/3 and 1/2
	corner_position.x += (corner_index % 3) == 0 ? 0 : /* For now add manual width */ 200.0f;
	// The y position is shared between 0/1 and 2/3
	corner_position.y -= corner_index < 2 ? 0 : (MENU_OPTION_SPACING.y * menu_option_entities.size());
	registry.positions.insert(menu_corner_entity, corner_position);
	// The rest is normal
	registry.ignoredByCamera.emplace(menu_corner_entity);
	registry.textured.insert(menu_corner_entity, { TEXTURE_ASSET_ID::UI_MENU_CORNER });
	registry.renderRequests.insert(
		menu_corner_entity,
		{
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::UI,
		});

	return menu_corner_entity;
}
