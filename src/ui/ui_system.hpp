#pragma once
#include "ecs/system.hpp"
#include "ui_components.hpp"
#include "ui_constants.hpp"
#include <map>
#include "menus/pause_menu.hpp"

class UiSystem : public SystemInterface
{
public:
	UiSystem()
	{
		menu_references[MENU_TYPE::PAUSE_MENU] = &pause_menu;
	}

	void step(float elapsed_ms);

private:
	std::map<MENU_TYPE, Menu*> menu_references;
	PauseMenu pause_menu;
};
