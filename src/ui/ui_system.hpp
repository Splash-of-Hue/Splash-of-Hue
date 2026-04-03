#pragma once
#include "ecs/system.hpp"
#include "menus/pause_menu.hpp"
#include "screens/loading_screen.hpp"
#include "ui_components.hpp"
#include "ui_constants.hpp"
#include <map>

class UiSystem : public SystemInterface
{
public:
	UiSystem()
	{
		menu_references[UI_TYPE::PAUSE_MENU] = &pause_menu;
		screen_references[UI_TYPE::LOADING_SCREEN] = &loading_screen;
	}

	void step(float elapsed_ms);

private:
	std::map<UI_TYPE, Screen*> screen_references;
	std::map<UI_TYPE, Menu*> menu_references;
	PauseMenu pause_menu;
	LoadingScreen loading_screen;
};
