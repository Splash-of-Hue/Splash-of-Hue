#pragma once
#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "ui/ui_components.hpp"
#include "ui/ui_constants.hpp"
#include <functional>
#include <string>
#include <utility>
#include <vector>

class LoadingScreen : public Screen
{
public:
	void enable();
	void disable();

private:
	const vec2 LOADING_TEXT_POSITION = vec2(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX) * 0.5f - vec2(50, 0);
	const vec2 MENU_OPTION_SPACING = vec2(0, -50);
	int current_option_selected = 0;
	std::vector<Entity> menu_entities;
	std::vector<Entity> menu_option_entities;
	Entity menu_selector_entity;

	Entity create_backdrop();
	Entity create_menu_option(std::string text, int index);
	Entity create_menu_corner(int corner_index);
};
