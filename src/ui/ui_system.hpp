#pragma once
#include "ecs/system.hpp"
#include "ui_components.hpp"
#include "ui_constants.hpp"
#include <map>

class UiSystem : public SystemInterface
{
public:
	void init();
	void step(float elapsed_ms);

private:
	std::vector<Menu*> menus;
	std::map<MENU_TYPE, int> menu_indices;

	//std::map<MENU_TYPE, Menu*> menus;
};
