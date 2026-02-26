#include "controllers/player_controller_system.hpp"
#include "controls/control_constants.hpp"
#include "ecs/registry.hpp"
#include "world_init.hpp"
#include <iostream>

Entity PlayerControllerSystem::create_player()
{
	ColourState player_color_state;
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(player_color_state.curr_colour_state);
	//auto& player_entity = createPlayer({ 0, 0 }, 40.0f, 40.0f, color_from_state);
	Entity player_entity = Entity();
	//TODO: RD9Gsz6C change to constant spawning point
	registry.positions.insert(player_entity, { GRID_CELL_WIDTH_PX, GRID_CELL_HEIGHT_PX });
	registry.scales.insert(player_entity, vec2((float)0.9f * GRID_CELL_WIDTH_PX, (float)0.9 * GRID_CELL_HEIGHT_PX));
	registry.angles.emplace(player_entity);
	registry.angles.get(player_entity).degrees = 0.0f;
	registry.sizes.insert(player_entity, Size{ 40.0f, 40.0f });
	registry.colors.emplace(player_entity, color_from_state);
	registry.textured.emplace(player_entity);
	registry.textured.get(player_entity).used_texture = TEXTURE_ASSET_ID::PLAYER_TEXTURE_01;
	registry.renderRequests.insert(
		player_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		}
	);
	vec2 velocity = { 0.0f, 0.0f };
	player_movement = vec2(0);
	registry.players.emplace(player_entity);
	registry.velocities.insert(player_entity, velocity);
	registry.colourStates.emplace(player_entity, player_color_state);
	Collidable& collidable = registry.collidables.emplace(player_entity);
	collidable.inelastic = true;

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
	Entity& player_entity = registry.players.entities[0];
	vec2& player_velocity = registry.velocities.get(player_entity);
	player_movement = vec2(0);
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
}
