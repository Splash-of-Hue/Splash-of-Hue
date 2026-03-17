#include "common.hpp"
#include "ecs/registry.hpp"
#include "ui/menus/pause_menu.hpp"
#include "ui_components.hpp"
#include "ui_constants.hpp"
#include "ui_system.hpp"
#include <utility>

void UiSystem::init()
{
	//menus[MENU_TYPE::PAUSE_MENU] = &PauseMenu();
	menus.push_back(&PauseMenu());
	menu_indices[MENU_TYPE::PAUSE_MENU] = menus.size() - 1;
}

void UiSystem::step(float elapsed_ms)
{
	// Handle any change requests
	for (MenuToggleRequest& toggle_request : registry.menu_toggle_requests.components)
	{
		int menu_index = menu_indices.at(toggle_request.type);
		Menu* menu = menus.at(menu_index);
		//Menu* menu = menus.at(toggle_request.type);

		if (menu->active)
		{
			menu->disable();
		}
		else
		{
			menu->enable();
		}
	}

	for (MenuAction& action : registry.menu_actions.components)
	{
		//for (std::pair<const MENU_TYPE, Menu*>& menu_pair : menus)
		for (Menu*& menu: menus)
		{
			//Menu* menu = menu_pair.second;

			if (!menu->active)
			{
				continue;
			}

			if (action.type == MENU_ACTION::CONFIRM)
			{
				menu->select();
			}
			else if (action.type == MENU_ACTION::CANCEL)
			{
				menu->cancel();
			}
			else if (action.type == MENU_ACTION::MOVE_UP)
			{
				menu->move(DIRECTION::UP);
			}
			else if (action.type == MENU_ACTION::MOVE_RIGHT)
			{
				menu->move(DIRECTION::RIGHT);
			}
			else if (action.type == MENU_ACTION::MOVE_DOWN)
			{
				menu->move(DIRECTION::DOWN);
			}
			else if (action.type == MENU_ACTION::MOVE_LEFT)
			{
				menu->move(DIRECTION::LEFT);
			}
		}
	}

}