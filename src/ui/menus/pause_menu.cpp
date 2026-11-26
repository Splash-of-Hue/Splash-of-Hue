#pragma once
#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "game_state/game_state_constants.hpp"
#include "map/map_components.hpp"
#include "pause_menu.hpp"
#include "shapes/shape_build_init.hpp"
#include "text/text_constants.hpp"
#include "text/text_init.hpp"
#include "ui/ui_components.hpp"
#include "ui/ui_constants.hpp"
#include <functional>
#include <vector>

void PauseMenu::select()
{
	// Call the callback based on the selected option
	options[current_option_selected].second();
}

void PauseMenu::cancel()
{
	registry.ui_toggle_requests.insert(Entity(), { UI_TYPE::PAUSE_MENU });
}

void PauseMenu::move(DIRECTION direction)
{
	int index_shift = 0;

	if (direction == DIRECTION::UP)
	{
		index_shift = -1;
	}

	if (direction == DIRECTION::DOWN)
	{
		index_shift = 1;
	}

	// Make sure to wrap
	current_option_selected = (options.size() + current_option_selected + index_shift) % options.size();
	registry.positions.get(menu_selector_entity) = get_selector_position();
}

void PauseMenu::enable()
{
	current_option_selected = 0;
	Entity& backdrop_entity = create_backdrop();
	// Add to a list to keep track of what should be removed when disabled
	menu_entities.push_back(backdrop_entity);

	for (int i = 0; i < options.size(); i++)
	{
		// Create each menu item with the text
		std::pair<std::string, std::function<void()>> option = options[i];
		Entity& menu_option = create_menu_option(option.first, i);
		// Add to a list to remove when disabled
		menu_entities.push_back(menu_option);
		// Add to list to reference as needed for functionality
		menu_option_entities.push_back(menu_option);
	}

	// Create the selector and add to the list to remove when disabled
	menu_entities.push_back(create_menu_selector_icon());

	// For each corner, create the pretty outline
	for (int i = 0; i < 4; i++)
	{
		menu_entities.push_back(create_menu_corner(i));
	}

	// Request state change to trigger
	registry.game_state_change_requests.insert(Entity(), { GAME_STATE::PAUSED });
	active = true;
}

void PauseMenu::disable()
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

Entity PauseMenu::create_menu_option(std::string text, int index)
{
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	const vec2 manual_offset = vec2(-40, 0);
	const auto get_menu_position = [&](int index)
	{
		return INITIAL_MENU_OPTION_POSITION + MENU_OPTION_SPACING * float(index) + manual_offset;
	};

	return create_text(text, get_menu_position(index), vec2(1), vec4(1), FONT_ID::BEAUTIDUL_HEARTBEAT);
}

Entity PauseMenu::create_backdrop()
{
	Entity& backdrop_entity = createRectangle(vec2(0), vec2(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), RENDER_LAYER::UI_BACKGROUND, TRANSLUCENT_BLACK_COLOUR);
	// Add to a list to destroy when disabled
	registry.renderRequests.get(backdrop_entity).layer = RENDER_LAYER::UI_BACKGROUND;
	registry.ignoredByCamera.emplace(backdrop_entity);

	return backdrop_entity;
}

Entity PauseMenu::create_menu_selector_icon()
{
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	const float selector_size = 50.0f;
	registry.angles.emplace(menu_selector_entity);
	registry.scales.insert(menu_selector_entity, vec2(selector_size));
	registry.sizes.insert(menu_selector_entity, { selector_size, selector_size });
	// Find the current option and place the selector based on that
	registry.positions.insert(menu_selector_entity, get_selector_position());
	registry.textured.insert(menu_selector_entity, { TEXTURE_ASSET_ID::UI_SELECTOR });
	registry.ignoredByCamera.emplace(menu_selector_entity);
	registry.renderRequests.insert(
		menu_selector_entity,
		{
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::UI,
		});

	return menu_selector_entity;
}

// Index: 0 = top left, 1 = top right, 2 = bottom right, 3 = bottom left
Entity PauseMenu::create_menu_corner(int corner_index)
{
	Entity menu_corner_entity;
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	const vec2 manual_offset = vec2(100, 40);
	const float corner_size = 50.0f;
	// Flip the angle on index 2 and 3
	float angle = corner_index < 2 ? 0 : 180;
	// Mirror on the x-axis for index 1 and 3
	float x_reflection = (corner_index % 2) == 0 ? 1 : -1;
	registry.angles.insert(menu_corner_entity, { angle });
	registry.scales.insert(menu_corner_entity, vec2(x_reflection * corner_size, corner_size));
	registry.sizes.insert(menu_corner_entity, { corner_size, corner_size });
	// Position is based on menu options which is screen space, so use world space equivalent
	vec2 corner_position = vec2(INITIAL_MENU_OPTION_POSITION.x, WINDOW_HEIGHT_PX - INITIAL_MENU_OPTION_POSITION.y) - manual_offset;
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	// The x position is shared between 0/3 and 1/2
	corner_position.x += (corner_index % 3) == 0 ? 0 : /* For now add manual width */ 200.0f;
	// The y position is shared between 0/1 and 2/3
	corner_position.y -= corner_index < 2 ? 0 : (MENU_OPTION_SPACING.y * menu_option_entities.size());
	registry.positions.insert(menu_corner_entity, corner_position);
	// The rest is normal
	registry.ignoredByCamera.emplace(menu_corner_entity);
	registry.textured.insert(menu_corner_entity, { TEXTURE_ASSET_ID::UI_CORNER_DECORATION });
	registry.renderRequests.insert(
		menu_corner_entity,
		{
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER::UI,
		});

	return menu_corner_entity;
}

vec2 PauseMenu::get_selector_position()
{
	// Get the entity of the currently selected option
	Entity& current_option_entity = menu_option_entities.at(current_option_selected);
	// The selection is text which is screen space,
	// while the selector is a texture which uses world space,
	// so translate to screen space
	vec2 selector_position = registry.positions.get(current_option_entity);
	selector_position = vec2(selector_position.x, WINDOW_HEIGHT_PX - selector_position.y);
	// TODO HE4XQcbZ: Remove magic numbers and generate dynamically
	//We also need to offset a little bit
	selector_position -= vec2(30, 20);

	return selector_position;
}
