#include "controllers/player_controller_system.hpp"
#include "controls/control_constants.hpp"
#include "ecs/registry.hpp"

void PlayerControllerSystem::step(float elapsed_ms)
{
	// If there isn't a player yet, wait till there is before controlling it
	if (registry.players.size() == 0)
	{
		return;
	}

	// Reset player velocity each step
	Entity& player_entity = registry.players.entities[0];
	vec2& player_velocity = registry.velocities.get(player_entity);
	vec2 player_movement = vec2(0);
	player_velocity = vec2(0);

	for (Entity& entity : registry.controls.entities)
	{
		Control& control = registry.controls.get(entity);

		//// Movement, on push sets the corresponding bitset value to true and on release sets it to false
		if (control.type == CONTROL_TYPE::MOVE)
		{
			if (control.phase != CONTROL_PHASE::END)
			{
				player_movement += vec2(control.value_1, control.value_2);
			}
		}
	}

	Player& player = registry.players.components[0];
	player_velocity += player_movement * player.speed;
	//std::cout << "DEBUG: Moving x: " << player_velocity.x << ", y: " << player_velocity.y << std::endl;
	
	bool is_moving = player_movement.x != 0 || player_movement.y != 0;

	ANIMATION_TRIGGER desired = is_moving ? ANIMATION_TRIGGER::WALK : ANIMATION_TRIGGER::IDLE;

	Animatable& anim = registry.animatables.get(player_entity);
	if (anim.current_trigger != desired) {
		if (is_moving)
		{
			anim.frame_rate = is_moving ? 5 : 3;  // walking : idle
		}

		if (registry.animation_triggers.has(player_entity)) {
			registry.animation_triggers.get(player_entity).animation_trigger = desired;
		}
		else {
			registry.animation_triggers.insert(player_entity, { desired });
		}
	}
}
