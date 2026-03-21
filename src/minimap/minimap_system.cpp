#include "minimap/minimap_system.hpp"
#include "minimap/minimap_components.hpp"
#include "minimap/minimap_constants.hpp"
#include "ecs/registry.hpp"
#include "map/map_components.hpp"
#include "player/player_components.hpp"
#include "renderer/render_constants.hpp"
#include <algorithm>
#include <iostream>

void MinimapSystem::step(float /*elapsed_ms*/)
{
	if (registry.levels.size() == 0)
		return;

	// don't run until at least one room has loaded MinimapReference
	const bool any_valid = std::any_of(
		registry.levels.components.begin(),
		registry.levels.components.end(),
		[](const Level& level) { return level.floor_ldtk_size.x > 1 && level.floor_ldtk_size.y > 1; }
	);

	if (!any_valid || registry.players.entities.empty())
		return;

	Entity player_entity = registry.players.entities[0];
	if (!registry.currentLevels.has(player_entity))
		return;
	// update current depth according to what depth player is on
	int player_level_uid = registry.currentLevels.get(player_entity).level_uid;
	for (const Level& level : registry.levels.components){
		if (level.uid == player_level_uid){
			current_depth = level.depth;
			break;
		}
	}

	auto [ldtk_dungeon_top_left, ldtk_dungeon_bot_right] = compute_ldtk_world_bounds();
	ivec2 ldtk_world_size = ldtk_dungeon_bot_right - ldtk_dungeon_top_left;

	// compute uniform minimap scale
	float scale_x = MINIMAP_MAX_SIZE / static_cast<float>(ldtk_world_size.x);
	float scale_y = MINIMAP_MAX_SIZE / static_cast<float>(ldtk_world_size.y);
	float minimap_scale = std::min(scale_x, scale_y);

	// total_map_w is the minimap's actual pixel width on screen
	float total_map_w = ldtk_world_size.x * minimap_scale;

	sync_minimap_rooms(ldtk_dungeon_top_left, minimap_scale, total_map_w);
	update_depth_visibility();

	// player icon, create once
	if (!player_icon_created)
	{
		player_icon_created = true;
		create_hud_rect_entity(
			player_icon_entity,
			vec2(0.f),
			MINIMAP_ICON_SIZE, MINIMAP_ICON_SIZE,
			MINIMAP_COLOR_PLAYER
		);
		registry.renderRequests.get(player_icon_entity).layer = RENDER_LAYER::UI;
		registry.minimapPlayerIcons.emplace(player_icon_entity);
	}

	// reposition player icon according to current room (stays in middle of the rectangle room icon)
	update_player_icon(player_level_uid, ldtk_dungeon_top_left, minimap_scale, total_map_w);
}

void MinimapSystem::create_hud_rect_entity(Entity entity, vec2 top_left, float w, float h, const vec4& color)
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
void MinimapSystem::update_hud_rect_entity(Entity entity, vec2 top_left, float w, float h, const vec4& color)
{
	registry.positions.get(entity) = top_left + vec2(w, h) * 0.5f;
	registry.scales.get(entity) = vec2(w, h);
	registry.sizes.get(entity) = { w, h };
	registry.colors.get(entity) = color;
}

Entity MinimapSystem::create_room_entity()
{
	Entity e;
	create_hud_rect_entity(e, vec2(0.f), 1.f, 1.f, MINIMAP_COLOR_VISITED);
	return e;
}

// converts positions of each level(room) / hallway from LDtk coords to minimap coords
vec2 MinimapSystem::ldtk_to_minimap(ivec2 ldtk_pos, ivec2 ldtk_dungeon_top_left, float minimap_scale, float total_map_w)
{
	vec2 local = vec2(ldtk_pos - ldtk_dungeon_top_left) * minimap_scale;
	return vec2(
		MINIMAP_ANCHOR_X - total_map_w + local.x,
		MINIMAP_ANCHOR_Y + local.y
	);
}

std::vector<Entity> MinimapSystem::create_hallway_entities(
	const Level& level,
	ivec2 ldtk_dungeon_top_left,
	float minimap_scale,
	float total_map_w)
{
	std::vector<Entity> hallway_entities;
	for (const auto& [hall_pos, hall_size] : level.hallway_ldtk_rects)
	{
		if (hall_size.x < HALL_SIZE_MIN || hall_size.y < HALL_SIZE_MIN) {
			continue;
		}
		Entity hall_entity;
		create_hud_rect_entity(hall_entity, vec2(0.f), 1.f, 1.f, MINIMAP_COLOR_VISITED);
		vec2 hall_top_left = ldtk_to_minimap(hall_pos, ldtk_dungeon_top_left, minimap_scale, total_map_w);
		float hall_w = hall_size.x * minimap_scale;
		float hall_h = hall_size.y * minimap_scale;
		update_hud_rect_entity(hall_entity, hall_top_left, hall_w, hall_h, MINIMAP_COLOR_VISITED);
		hallway_entities.push_back(hall_entity);
	}
	return hallway_entities;
}

void MinimapSystem::update_hallway_entities(
	const std::vector<Entity>& hallway_render_entities,
	const Level& level,
	ivec2 ldtk_dungeon_top_left,
	float minimap_scale,
	float total_map_w)
{
	int valid_index = 0;
	for (const auto& [hall_pos, hall_size] : level.hallway_ldtk_rects)
	{
		if (hall_size.x < HALL_SIZE_MIN || hall_size.y < HALL_SIZE_MIN) {
			continue;
		}
		if (valid_index >= static_cast<int>(hallway_render_entities.size())) {
			break;
		}
		vec2 hall_top_left = ldtk_to_minimap(hall_pos, ldtk_dungeon_top_left, minimap_scale, total_map_w);
		float hall_w = hall_size.x * minimap_scale;
		float hall_h = hall_size.y * minimap_scale;
		update_hud_rect_entity(hallway_render_entities[valid_index], hall_top_left, hall_w, hall_h, MINIMAP_COLOR_VISITED);
		valid_index++;
	}
}

// compute LDtk world bounds (of the entire dungeon) to scale and position minimap
std::pair<ivec2, ivec2> MinimapSystem::compute_ldtk_world_bounds()
{
	ivec2 ldtk_dungeon_top_left = registry.levels.components[0].floor_ldtk_position;
	ivec2 ldtk_dungeon_bottom_right = registry.levels.components[0].floor_ldtk_position
		+ registry.levels.components[0].floor_ldtk_size;

	for (const Level& level : registry.levels.components)
	{
		if (level.depth != current_depth) {
			continue;
		}
		ldtk_dungeon_top_left = ivec2(
			std::min(ldtk_dungeon_top_left.x, level.floor_ldtk_position.x),
			std::min(ldtk_dungeon_top_left.y, level.floor_ldtk_position.y)
		);
		ldtk_dungeon_bottom_right = ivec2(
			std::max(ldtk_dungeon_bottom_right.x, level.floor_ldtk_position.x + level.floor_ldtk_size.x),
			std::max(ldtk_dungeon_bottom_right.y, level.floor_ldtk_position.y + level.floor_ldtk_size.y)
		);
	}
	return { ldtk_dungeon_top_left, ldtk_dungeon_bottom_right };
}

void MinimapSystem::sync_minimap_rooms(ivec2 ldtk_dungeon_top_left, float minimap_scale, float total_map_w)
{
	for (int i = 0; i < static_cast<int>(registry.levels.entities.size()); i++)
	{
		Entity level_entity = registry.levels.entities[i];
		const Level& level = registry.levels.components[i];
		if (level.depth != current_depth) {
			continue;
		}
		vec2 room_top_left = ldtk_to_minimap(level.floor_ldtk_position, ldtk_dungeon_top_left, minimap_scale, total_map_w);
		float room_w = level.floor_ldtk_size.x * minimap_scale;
		float room_h = level.floor_ldtk_size.y * minimap_scale;
		vec4 room_color = MINIMAP_COLOR_VISITED;

		if (!registry.minimapRooms.has(level_entity)) {
			create_minimap_room(level_entity, level, room_top_left, room_w, room_h, room_color, ldtk_dungeon_top_left, minimap_scale, total_map_w);
		}
		else {
			update_minimap_room(level_entity, level, room_top_left, room_w, room_h, room_color, ldtk_dungeon_top_left, minimap_scale, total_map_w);
		}
	}
}

void MinimapSystem::create_minimap_room(
	Entity level_entity,
	const Level& level,
	vec2 room_top_left,
	float room_w,
	float room_h,
	const vec4& room_color,
	ivec2 ldtk_dungeon_top_left,
	float minimap_scale,
	float total_map_w)
{
	if (level.floor_ldtk_size.x <= 1 || level.floor_ldtk_size.y <= 1)
		return;

	if (!level.is_visited)
	{
		// create placeholder 
		registry.minimapRooms.insert(level_entity, {
			Entity(),
			false,
			{}
		});
		return;
	}

	Entity room_render_entity = create_room_entity();
	update_hud_rect_entity(room_render_entity, room_top_left, room_w, room_h, room_color);
	std::vector<Entity> hallway_entities = create_hallway_entities(level, ldtk_dungeon_top_left, minimap_scale, total_map_w);

	registry.minimapRooms.insert(level_entity, {
		room_render_entity,
		true,
		hallway_entities
		});
}

void MinimapSystem::update_minimap_room(
	Entity level_entity,
	const Level& level,
	vec2 room_top_left,
	float room_w,
	float room_h,
	const vec4& room_color,
	ivec2 ldtk_dungeon_top_left,
	float minimap_scale,
	float total_map_w)
{
	MinimapRoom& minimap_room = registry.minimapRooms.get(level_entity);

	if (level.is_visited && !minimap_room.has_render_entity)
	{
		// room just became visited, create render entities for it
		Entity room_render_entity = create_room_entity();
		update_hud_rect_entity(room_render_entity, room_top_left, room_w, room_h, room_color);
		minimap_room.room_render_entity = room_render_entity;
		minimap_room.has_render_entity = true;
		minimap_room.hallway_render_entities = create_hallway_entities(level, ldtk_dungeon_top_left, minimap_scale, total_map_w);
	}
	else if (level.is_visited && minimap_room.has_render_entity)
	{
		// room is visited and already has render entity, just update position/scale
		update_hud_rect_entity(minimap_room.room_render_entity, room_top_left, room_w, room_h, room_color);
		update_hallway_entities(minimap_room.hallway_render_entities, level, ldtk_dungeon_top_left, minimap_scale, total_map_w);
	}
}

void MinimapSystem::update_player_icon(
	int player_level_uid,
	ivec2 ldtk_dungeon_top_left,
	float minimap_scale,
	float total_map_w)
{
	const auto player_match = [&](const Level& level) { return level.uid == player_level_uid; };
	auto it = std::find_if(
		registry.levels.components.begin(),
		registry.levels.components.end(),
		player_match);

	if (it == registry.levels.components.end()) {
		return;
	}
	vec2 room_top_left = ldtk_to_minimap(it->floor_ldtk_position, ldtk_dungeon_top_left, minimap_scale, total_map_w);
	float room_w = it->floor_ldtk_size.x * minimap_scale;
	float room_h = it->floor_ldtk_size.y * minimap_scale;
	vec2 icon_top_left = room_top_left + (vec2(room_w, room_h) - vec2(MINIMAP_ICON_SIZE)) * 0.5f;
	registry.positions.get(player_icon_entity) = icon_top_left + vec2(MINIMAP_ICON_SIZE) * 0.5f;
}

void MinimapSystem::set_depth_visibility(Entity render_entity, bool visible)
{
	bool has_render = registry.renderRequests.has(render_entity);
	if (visible && !has_render)
		registry.renderRequests.insert(render_entity, {
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			RENDER_LAYER::HUD
			});
	else if (!visible && has_render)
		registry.renderRequests.remove(render_entity);
}

void MinimapSystem::update_depth_visibility()
{
	for (int i = 0; i < static_cast<int>(registry.levels.entities.size()); i++)
	{
		Entity level_entity = registry.levels.entities[i];
		const Level& level = registry.levels.components[i]; 

		if (!registry.minimapRooms.has(level_entity))
			continue;

		MinimapRoom& minimap_room = registry.minimapRooms.get(level_entity);
		if (!minimap_room.has_render_entity)
			continue;

		bool visible = (level.depth == current_depth);
		set_depth_visibility(minimap_room.room_render_entity, visible);
		for (Entity hallway_entity : minimap_room.hallway_render_entities)
			set_depth_visibility(hallway_entity, visible);
	}
}
