#pragma once
#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "map/map_components.hpp"
#include <vector>
#include <utility>

class MinimapSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);

private:
	Entity player_icon_entity;
	bool player_icon_created = false;

	static std::vector<Entity> create_hallway_entities(const Level& level, ivec2 ldtk_world_min, float minimap_scale, float total_map_w);
	static Entity create_room_entity();
	static vec2 ldtk_to_minimap(ivec2 ldtk_pos, ivec2 ldtk_world_min, float minimap_scale, float total_map_w);
	static void create_hud_rect_entity(Entity entity, vec2 top_left, float w, float h, const vec4& color);
	static void update_hallway_entities(const std::vector<Entity>& hallway_render_entities, const Level& level, ivec2 ldtk_world_min, float minimap_scale, float total_map_w);
	static void update_hud_rect_entity(Entity entity, vec2 top_left, float w, float h, const vec4& color);

	std::pair<ivec2, ivec2> compute_ldtk_world_bounds();
	void create_minimap_room(Entity level_entity, const Level& level, vec2 room_top_left, float room_w, float room_h, const vec4& room_color, ivec2 ldtk_world_min, float minimap_scale, float total_map_w);
	void sync_minimap_rooms(ivec2 ldtk_world_min, float minimap_scale, float total_map_w);
	void update_minimap_room(Entity level_entity, const Level& level, vec2 room_top_left, float room_w, float room_h, const vec4& room_color, ivec2 ldtk_world_min, float minimap_scale, float total_map_w);
	void update_player_icon(int player_level_uid, ivec2 ldtk_world_min, float minimap_scale, float total_map_w);
};
