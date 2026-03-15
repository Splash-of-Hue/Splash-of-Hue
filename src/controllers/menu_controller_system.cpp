#include "controllers/menu_controller_system.hpp"
#include "controls/control_constants.hpp"
#include "ecs/registry.hpp"

void MenuControllerSystem::step(float elapsed_ms)
{
	for (Entity& entity : registry.controls.entities)
	{
		Control& control = registry.controls.get(entity);

		if (control.type == CONTROL_TYPE::PAUSE)
		{
			if (control.phase != CONTROL_PHASE::END)
			{
				player_movement += vec2(control.value_1, control.value_2);
			}
		}
	}

}
