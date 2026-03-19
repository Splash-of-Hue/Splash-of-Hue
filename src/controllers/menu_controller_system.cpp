#include "controllers/menu_controller_system.hpp"
#include "controls/control_constants.hpp"
#include "ecs/registry.hpp"
#include "game_state/game_state_constants.hpp"

void MenuControllerSystem::step(float elapsed_ms)
{
	vec2 input_movement = vec2(0);

	for (Entity& entity : registry.controls.entities)
	{
		Control& control = registry.controls.get(entity);

		// There are no actions we wish to take not at the end so far
		if (control.phase != CONTROL_PHASE::END)
		{
			continue;
		}

		if (control.type == CONTROL_TYPE::PAUSE)
		{
			registry.menu_toggle_requests.insert(Entity(), { MENU_TYPE::PAUSE_MENU });
		}
		else if (control.type == CONTROL_TYPE::MOVE)
		{
			// Add all the input, each axis is calculated seperately
			input_movement += vec2(control.value_1, control.value_2);
		}
		else if (control.type == CONTROL_TYPE::CONFIRM)
		{
			registry.menu_actions.insert(Entity(), { MENU_ACTION::CONFIRM });
		}
		else if (control.type == CONTROL_TYPE::CANCEL)
		{
			registry.menu_actions.insert(Entity(), { MENU_ACTION::CANCEL });
		}
	}

	// If neither input is large enough to matter, we are done
	if (max(abs(input_movement.x), abs(input_movement.y)) < MENU_MOVE_STOCK_DEADZONE)
	{
		return;
	}

	MENU_ACTION menu_action = MENU_ACTION::INVALID;

	// Check which axis is bigger
	if (abs(input_movement.x) > abs(input_movement.y))
	{
		menu_action = input_movement.x < 0 ? MENU_ACTION::MOVE_LEFT : MENU_ACTION::MOVE_RIGHT;
	}
	else
	{
		menu_action = input_movement.y < 0 ? MENU_ACTION::MOVE_UP : MENU_ACTION::MOVE_DOWN;
	}

	registry.menu_actions.insert(Entity(), { menu_action });
}
