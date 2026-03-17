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

class PauseMenu : public Menu
{
public:
	void select();
	void cancel();
	void move(DIRECTION direction);
	void enable();
	void disable();

private:
	const vec2 INITIAL_MENU_OPTION_POSITION = vec2(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX) * 0.5f;
	const vec2 MENU_OPTION_SPACING = vec2(0, -50);
	int current_option_selected = 0;
	std::vector<Entity> menu_entities;
	std::vector<Entity> menu_option_entities;
	Entity menu_selector_entity;
	std::vector<std::pair<std::string, std::function<void()>>> options =
		{
		{
			"CONTINUE",
			[&]() { registry.menu_toggle_requests.insert(Entity(), { MENU_TYPE::PAUSE_MENU }); }
		},
		{
			"QUIT",
			[&]() { glfwSetWindowShouldClose(registry.windows.components[0].window_reference, GLFW_TRUE); }
		},
	};

	Entity create_backdrop();
	Entity create_menu_option(std::string text, int index);
	Entity create_menu_selector_icon();
	Entity create_menu_corner(int corner_index);
	vec2 get_selector_position();
};
