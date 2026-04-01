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
	std::vector<Entity> ui_entities;

	Entity create_backdrop();
	Entity create_ui_corner(int corner_index);
};
