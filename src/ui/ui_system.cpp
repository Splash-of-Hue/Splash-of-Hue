#include "common.hpp"
#include "ecs/registry.hpp"
#include "ui_components.hpp"
#include "ui_constants.hpp"
#include "ui_system.hpp"
#include <utility>

void UiSystem::step(float elapsed_ms)
{
	for (MenuAction& action : registry.menu_actions.components)
	{
		for (std::pair<UI_TYPE, Menu*> menu_pair: menu_references)
		{
			Menu* menu = menu_pair.second;

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

	// Handle any change requests
	for (UiToggleRequest& toggle_request : registry.ui_toggle_requests.components)
	{
		if (menu_references.count(toggle_request.type) > 0)
		{
			Menu* menu = menu_references.at(toggle_request.type);

			if (menu->active)
			{
				menu->disable();
			}
			else
			{
				menu->enable();
			}
		}

		if (screen_references.count(toggle_request.type) > 0)
		{
			Screen* screen = screen_references.at(toggle_request.type);

			if (screen->active)
			{
				screen->disable();
			}
			else
			{
				screen->enable();
			}
		}
	}

	registry.menu_actions.clear();
	registry.ui_toggle_requests.clear();
}
