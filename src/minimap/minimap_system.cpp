#include "minimap/minimap_system.hpp"
#include "minimap/minimap_components.hpp"
#include "minimap/minimap_constants.hpp"
#include "ecs/registry.hpp"
#include "map/map_components.hpp"
#include "player/player_components.hpp"
#include "renderer/render_constants.hpp"
#include <algorithm>
#include <iostream>

static void setup_hud_rect_entity(Entity entity, vec2 top_left, float w, float h, const vec4& color)
{
	registry.positions.insert(entity, top_left + vec2(w, h) * 0.5f);
	registry.scales.insert(entity, vec2(w, h));
	registry.sizes.insert(entity, { w, h });
	registry.angles.insert(entity, { 0.f });
	registry.colors.insert(entity, color);
	registry.ignoredByCamera.emplace(entity);
	registry.rectangles.emplace(entity);
	registry.renderRequests.insert(entity, {
		EFFECT_ASSET_ID::DEFAULT,
		GEOMETRY_BUFFER_ID::RECTANGLE,
		RENDER_LAYER::HUD
		});
}

// updates existing minimap rectangle to keep position/scale/size correct as player moves
static void update_hud_rect_entity(Entity entity, vec2 top_left, float w, float h, const vec4& color)
{
	registry.positions.get(entity) = top_left + vec2(w, h) * 0.5f;
	registry.scales.get(entity) = vec2(w, h);
	registry.sizes.get(entity) = { w, h };
	registry.colors.get(entity) = color;
}

Entity MinimapSystem::create_room_entity()
{
	Entity e;
	setup_hud_rect_entity(e, vec2(0.f), 1.f, 1.f, MINIMAP_COLOR_VISITED);
	return e;
}

// converts positions of each level(room) / hallway from LDtk coords to minimap coords
static vec2 ldtk_to_screen(ivec2 ldtk_pos, ivec2 ldtk_world_min, float minimap_scale, float total_map_w)
{
	vec2 local = vec2(ldtk_pos - ldtk_world_min) * minimap_scale;
	return vec2(
		MINIMAP_ANCHOR_X - total_map_w + local.x,
		MINIMAP_ANCHOR_Y + local.y
	);
}

static std::vector<Entity> create_hallway_entities(
	const Level& level,
	ivec2 ldtk_min,
	float minimap_scale,
	float total_map_w)
{
	std::vector<Entity> hallway_entities;
	for (const auto& [hall_pos, hall_size] : level.hallway_ldtk_rects)
	{
		if (hall_size.x < 32 || hall_size.y < 32)
			continue;
		Entity hall_entity;
		setup_hud_rect_entity(hall_entity, vec2(0.f), 1.f, 1.f, MINIMAP_COLOR_VISITED);
		vec2 hall_top_left = ldtk_to_screen(hall_pos, ldtk_min, minimap_scale, total_map_w);
		float hall_w = hall_size.x * minimap_scale;
		float hall_h = hall_size.y * minimap_scale;
		update_hud_rect_entity(hall_entity, hall_top_left, hall_w, hall_h, MINIMAP_COLOR_VISITED);
		hallway_entities.push_back(hall_entity);
	}
	return hallway_entities;
}

static void update_hallway_entities(
	const std::vector<Entity>& hallway_render_entities,
	const Level& level,
	ivec2 ldtk_min,
	float minimap_scale,
	float total_map_w)
{
	int valid_index = 0;
	for (const auto& [hall_pos, hall_size] : level.hallway_ldtk_rects)
	{
		if (hall_size.x < 32 || hall_size.y < 32)
			continue;
		if (valid_index >= (int)hallway_render_entities.size())
			break;
		vec2 hall_top_left = ldtk_to_screen(hall_pos, ldtk_min, minimap_scale, total_map_w);
		float hall_w = hall_size.x * minimap_scale;
		float hall_h = hall_size.y * minimap_scale;
		update_hud_rect_entity(hallway_render_entities[valid_index], hall_top_left, hall_w, hall_h, MINIMAP_COLOR_VISITED);
		valid_index++;
	}
}

void MinimapSystem::step(float elapsed_ms)
{
	if (registry.levels.size() == 0) {
		return;
	}

	// don't run until at least one room has loaded MinimapReference
	bool any_valid = false;
	for (const Level& lvl : registry.levels.components)
	{
		if (lvl.floor_ldtk_size.x > 1 && lvl.floor_ldtk_size.y > 1)
		{
			any_valid = true;
			break;
		}
	}
	if (!any_valid) {
		return;
	}
		
	int player_level_uid = -1;
	if (!registry.players.entities.empty())
	{
		Entity player_entity = registry.players.entities[0];
		if (registry.currentLevels.has(player_entity))
			player_level_uid = registry.currentLevels.get(player_entity).level_uid;
	}

	// compute LDtk world bounds (of the entire dungeon) to scale and position minimap 
	ivec2 ldtk_min = registry.levels.components[0].floor_ldtk_position;
	ivec2 ldtk_max = registry.levels.components[0].floor_ldtk_position
		+ registry.levels.components[0].floor_ldtk_size;

	for (const Level& lvl : registry.levels.components)
	{
		ldtk_min = ivec2(
			std::min(ldtk_min.x, lvl.floor_ldtk_position.x),
			std::min(ldtk_min.y, lvl.floor_ldtk_position.y)
		);
		ldtk_max = ivec2(
			std::max(ldtk_max.x, lvl.floor_ldtk_position.x + lvl.floor_ldtk_size.x),
			std::max(ldtk_max.y, lvl.floor_ldtk_position.y + lvl.floor_ldtk_size.y)
		);
	}

	ivec2 ldtk_world_size = ldtk_max - ldtk_min;

	// compute uniform minimap scale
	float scale_x = MINIMAP_MAX_SIZE / (float)ldtk_world_size.x;
	float scale_y = MINIMAP_MAX_SIZE / (float)ldtk_world_size.y;
	float minimap_scale = std::min(scale_x, scale_y);

	// total_map_w is the minimap's actual pixel width on screen, used for right-anchoring.
	float total_map_w = ldtk_world_size.x * minimap_scale;

	for (int i = 0; i < (int)registry.levels.entities.size(); i++)
	{
		Entity level_entity = registry.levels.entities[i];
		const Level& level = registry.levels.components[i];
		bool is_player_here = (level.uid == player_level_uid);

		vec2 room_top_left = ldtk_to_screen(level.floor_ldtk_position, ldtk_min, minimap_scale, total_map_w);
		float room_w = level.floor_ldtk_size.x * minimap_scale;
		float room_h = level.floor_ldtk_size.y * minimap_scale;
		vec4 room_color = MINIMAP_COLOR_VISITED;

		if (!registry.minimapRooms.has(level_entity))
		{
			if (level.floor_ldtk_size.x <= 1 || level.floor_ldtk_size.y <= 1) {
				continue;
			}
				
			if (!level.is_visited)
			{
				// Room is not yet visited, make placeholder
				registry.minimapRooms.insert(level_entity, {
					level.floor_ldtk_position,
					level.floor_ldtk_size,
					Entity(),
					false,
					{}
				});
				continue;
			}

			// visited, so create room and hallway entities
			Entity room_render_entity = create_room_entity();
			update_hud_rect_entity(room_render_entity, room_top_left, room_w, room_h, room_color);
			std::vector<Entity> hallway_entities = create_hallway_entities(level, ldtk_min, minimap_scale, total_map_w);

			registry.minimapRooms.insert(level_entity, {
				level.floor_ldtk_position,
				level.floor_ldtk_size,
				room_render_entity,
				true,
				hallway_entities
			});
		}
		else
		{
			// minimap already exists, update it
			MinimapRoom& minimap_room = registry.minimapRooms.get(level_entity);
			minimap_room.floor_ldtk_position = level.floor_ldtk_position;
			minimap_room.floor_ldtk_size = level.floor_ldtk_size;

			if (level.is_visited && !minimap_room.has_render_entity)
			{
				// room just became visited, create render entities for it
				Entity room_render_entity = create_room_entity();
				update_hud_rect_entity(room_render_entity, room_top_left, room_w, room_h, room_color);
				minimap_room.room_render_entity = room_render_entity;
				minimap_room.has_render_entity = true;
				minimap_room.hallway_render_entities = create_hallway_entities(level, ldtk_min, minimap_scale, total_map_w);
			}
			else if (level.is_visited && minimap_room.has_render_entity)
			{
				// room is visited and already has render entity, just update position/scale 
				update_hud_rect_entity(minimap_room.room_render_entity, room_top_left, room_w, room_h, room_color);
				update_hallway_entities(minimap_room.hallway_render_entities, level, ldtk_min, minimap_scale, total_map_w);
			}
		}
	}

	// player icon, reposition each step
	if (!player_icon_created && player_level_uid != -1)
	{
		player_icon_created = true;
		setup_hud_rect_entity(
			player_icon_entity,
			vec2(0.f),
			MINIMAP_ICON_SIZE, MINIMAP_ICON_SIZE,
			MINIMAP_COLOR_PLAYER
		);
		registry.minimapPlayerIcons.emplace(player_icon_entity);
	}

	// reposition player icon according to current room (stays in middle of the rectangle room icon)
	if (player_icon_created && player_level_uid != -1)
	{
		const auto player_match = [&](const Level& l) { return l.uid == player_level_uid; };
		auto pit = std::find_if(
			registry.levels.components.begin(),
			registry.levels.components.end(), player_match);
		if (pit != registry.levels.components.end())
		{
			vec2 room_top_left = ldtk_to_screen(pit->floor_ldtk_position, ldtk_min, minimap_scale, total_map_w);
			float room_w = pit->floor_ldtk_size.x * minimap_scale;
			float room_h = pit->floor_ldtk_size.y * minimap_scale;
			vec2 icon_top_left = room_top_left + (vec2(room_w, room_h) - vec2(MINIMAP_ICON_SIZE)) * 0.5f;
			registry.positions.get(player_icon_entity) = icon_top_left + vec2(MINIMAP_ICON_SIZE) * 0.5f;
		}
	}
}
