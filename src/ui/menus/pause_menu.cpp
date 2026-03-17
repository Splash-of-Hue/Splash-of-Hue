#pragma once
#include "common.hpp"
#include "ecs/registry.hpp"
#include "shapes/shape_build_init.hpp"
#include "text/text_constants.hpp"
#include "text/text_init.hpp"
#include "ui/ui_components.hpp"
#include <functional>
#include <vector>
#include "pause_menu.hpp"

void PauseMenu::select()
{
	// Call the callback based on the selected option
	options[current_option_selected].second();
}

void PauseMenu::cancel()
{
	registry.menu_toggle_requests.insert(Entity(), { MENU_TYPE::PAUSE_MENU });
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
	current_option_selected = (current_option_selected + index_shift) % options.size();

	// TODO: Move graphical selectors, for testing, change color

}

void PauseMenu::enable()
{
	Entity backdrop_entity = createRectangle(vec2(0), vec2(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX), RENDER_LAYER::UI_BACKGROUND, TRANSLUCENT_BLACK_COLOUR);
	// Add to a list to destroy when disabled
	menu_entities.push_back(backdrop_entity);

	for (int i = 0; i < options.size(); i++)
	{
		// Create each menu item with the text
		std::pair<std::string, std::function<void()>> option = options[i];
		Entity& menu_option = create_menu_option(option.first, i);
		// Add to a list to destroy when disabled
		menu_entities.push_back(menu_option);
	}

	active = true;
}

void PauseMenu::disable()
{
	for (Entity entity : menu_entities)
	{
		registry.remove_all_components_of(entity);
	}

	menu_entities.clear();
	active = false;
}

Entity PauseMenu::create_menu_option(std::string text, int index)
{
	const auto get_menu_position = [&](int index)
	{
		return INITIAL_MENU_OPTION_POSITION + MENU_OPTION_SPACING * float(index);
	};

	// TODO: Add render layer to text
	return create_text(text, get_menu_position(index), vec2(1), vec4(1), FONT_ID::BEAUTIDUL_HEARTBEAT);
}

Entity PauseMenu::create_menu_corners()
{
	return Entity();
}
