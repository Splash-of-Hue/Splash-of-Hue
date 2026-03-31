#include "controllers/player_controller_system.hpp"
#include "controls/control_constants.hpp"
#include "ecs/registry.hpp"
#include "player_system.hpp"

void PlayerSystem::step(float elapsed_ms)
{
	if (registry.playerCreateRequests.size() > 0)
	{
		PlayerCreateRequest create_request = registry.playerCreateRequests.components.back();
		create_player(create_request.level_uid, create_request.starting_position);
		registry.playerCreateRequests.clear();
	}
}

void PlayerSystem::create_player(int level_uid, vec2 position)
{
	// Start by checking if there is a player already loaded
	if (registry.players.size() > 0)
	{
		// We have a loaded player, don't make another one
		return;
	}

	Entity player_entity;
	ColourState player_color_state;
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(player_color_state.curr_colour_state);
	Animatable& animatable = registry.animatables.emplace(player_entity);
	animatable.type = ANIMATABLE_TYPE::PLAYER;
	animatable.behavior = ANIMATION_BEHAVIOR::LOOP;
	animatable.current_trigger = ANIMATION_TRIGGER::IDLE;
	animatable.frame_rate = 3;
	animatable.cancellable = true;
	registry.positions.emplace(player_entity, position);
	registry.scales.insert(player_entity, vec2((float)2.0f * GRID_CELL_WIDTH_PX, (float)2.0f * GRID_CELL_HEIGHT_PX));
	registry.angles.emplace(player_entity);
	registry.angles.get(player_entity).degrees = 0.0f;
	Size& size = registry.sizes.insert(player_entity, Size{ 40.0f, 40.0f });
	registry.colors.emplace(player_entity, color_from_state);
	registry.currentLevels.insert(player_entity, { level_uid });
	registry.textured.emplace(player_entity);
	registry.textured.get(player_entity).used_texture = TEXTURE_ASSET_ID::PLAYER_TEXTURE_01;
	registry.renderRequests.insert(
		player_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::PLAYER,
		}
		);
	vec2 velocity = { 0.0f, 0.0f };
	registry.players.emplace(player_entity);
	registry.velocities.insert(player_entity, velocity);
	registry.colourStates.emplace(player_entity, player_color_state);
	Collidable& collidable = registry.collidables.emplace(player_entity);
	collidable.inelastic = true;
	// Add mesh to player, default is triangle
	PolygonMesh& polygon = registry.hitboxes.emplace(player_entity);
	polygon.centre = position;
	polygon.vertices.push_back(vec2(position.x + size.width * 0.45f, position.y - size.height * 0.45f)); 
	polygon.vertices.push_back(vec2(position.x - size.width * 0.45f, position.y - size.height * 0.45f)); 
	polygon.vertices.push_back(vec2(position.x + size.width * 0.45f, position.y + size.height * 0.45f));
	polygon.vertices.push_back(vec2(position.x - size.width * 0.45f, position.y + size.height * 0.45f)); 
	polygon.base_vertices = polygon.vertices;
}
