#include "controllers/player_controller_system.hpp"
#include "controls/control_constants.hpp"
#include "ecs/registry.hpp"
#include <iostream>

Entity PlayerControllerSystem::create_player()
{
	Entity player_entity;
	registry.players.emplace(player_entity);
	registry.motions.emplace(player_entity);
	registry.positions.emplace(player_entity);
	player_movement = vec2(0);

	return player_entity;
}

void PlayerControllerSystem::step(float elapsed_ms)
{
	if (registry.playerCreateRequests.size() > 0)
	{
		// Don't make a second player
		assert(registry.players.size() == 0);
		create_player();
		registry.playerCreateRequests.clear();
	}

	// If there isn't a player yet, wait till there is before controlling it
	if (registry.players.size() == 0)
	{
		return;
	}

	// Reset player velocity each step
	auto& player_entity = registry.players.entities[0];
	auto& player_velocity = registry.motions.get(player_entity).velocity;
	player_velocity = vec2(0);

	for (auto& entity : registry.controls.entities)
	{
		auto& control = registry.controls.get(entity);

		//// Movement, on push sets the corresponding bitset value to true and on release sets it to false
		if (control.type == CONTROL_TYPE::MOVE)
		{
			if (control.phase == CONTROL_PHASE::START)
			{
				player_movement += vec2(control.value_1, control.value_2);
			}
			else if (control.phase == CONTROL_PHASE::END)
			{
				player_movement -= vec2(control.value_1, control.value_2);
			}
		}
	}

	player_velocity += player_movement;
	std::cout << "DEBUG: Moving x: " << player_velocity.x << ", y: " << player_velocity.y << std::endl;
}
