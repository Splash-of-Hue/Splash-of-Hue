#include "animation/animation_components.hpp"
#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "map/map_build_init.hpp"
#include "map_components.hpp"
#include "renderer/render_constants.hpp"
#include "shapes/shape_build_init.hpp"
#include <string>

Entity createMossWall(const int level_uid, std::string id, vec2 position, vec2 size, COLOUR_STATE color_state)
{
	Entity moss_wall_entity;
	registry.positions.insert(moss_wall_entity, position);
	registry.scales.insert(moss_wall_entity, size);
	registry.sizes.insert(moss_wall_entity, { size.x, size.y });
	registry.angles.emplace(moss_wall_entity);
	registry.unloadables.emplace(moss_wall_entity, Unloadable{ position });
	Collidable& col = registry.collidables.emplace(moss_wall_entity);
	col.inelastic = true;
	registry.colouredWall.insert(moss_wall_entity, { id });
	registry.colourStates.insert(moss_wall_entity, { color_state, color_state });
	std::map<COLOUR_STATE, ANIMATION_TRIGGER> colour_animation_trigger_map =
	{
		{ COLOUR_STATE::NEUTRAL, ANIMATION_TRIGGER::IDLE },
		{ COLOUR_STATE::RED, ANIMATION_TRIGGER::IDLE_RED },
		{ COLOUR_STATE::BLUE, ANIMATION_TRIGGER::IDLE_BLUE },
	};
	registry.renderRequests.insert(
		moss_wall_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::WALLS_FRONT
		}
	);
	const Animatable animatable = {
		ANIMATABLE_TYPE::MOSS_WALL,
		ANIMATION_BEHAVIOR::LOOP,
		colour_animation_trigger_map[color_state],
		1
	};
	TEXTURE_ASSET_ID initial_texture = color_state == COLOUR_STATE::NEUTRAL ? TEXTURE_ASSET_ID::MOSS_NEUTRAL : TEXTURE_ASSET_ID::MOSS;
	registry.animatables.insert(moss_wall_entity, animatable);
	registry.textured.insert(moss_wall_entity, { initial_texture });

	return moss_wall_entity;
}

Entity createMossIndicator(const int level_uid, std::string id, vec2 position, vec2 size, COLOUR_STATE color_state)
{
	Entity indicator_entity;
	registry.positions.insert(indicator_entity, position);
	registry.scales.insert(indicator_entity, size);
	registry.sizes.insert(indicator_entity, { size.x, size.y });
	registry.angles.emplace(indicator_entity);
	registry.unloadables.emplace(indicator_entity, Unloadable{ position });
	registry.colouredWall.insert(indicator_entity, { id });
	registry.colourStates.insert(indicator_entity, { color_state, color_state });

	std::map<COLOUR_STATE, ANIMATION_TRIGGER> colour_animation_trigger_map =
	{
		{ COLOUR_STATE::NEUTRAL, ANIMATION_TRIGGER::IDLE },
		{ COLOUR_STATE::RED, ANIMATION_TRIGGER::IDLE_RED },
		{ COLOUR_STATE::BLUE, ANIMATION_TRIGGER::IDLE_BLUE },
	};

	registry.renderRequests.insert(
		indicator_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR_INTERACTABLES
		}
	);

	const Animatable animatable = {
		ANIMATABLE_TYPE::MOSS_INDICATOR,
		ANIMATION_BEHAVIOR::LOOP,
		colour_animation_trigger_map[color_state],
		1
	};
	registry.animatables.insert(indicator_entity, animatable);
	registry.textured.insert(indicator_entity, { TEXTURE_ASSET_ID::MOSS_INDICATOR });

	return indicator_entity;
}

Entity createPressurePlateWall(const int level_uid, std::string id, vec2 position, vec2 size, bool is_open)
{
		Entity pressure_plate_wall_entity;
		PressurePlateWall& pressure_plate_wall = registry.pressurePlateWalls.emplace(pressure_plate_wall_entity);
		pressure_plate_wall.is_open = is_open;
		pressure_plate_wall.id = id;

		registry.positions.insert(pressure_plate_wall_entity, position);
		registry.scales.insert(pressure_plate_wall_entity, size);
		registry.sizes.insert(pressure_plate_wall_entity, { size.x, size.y });
		registry.walls.emplace(pressure_plate_wall_entity);
		registry.angles.emplace(pressure_plate_wall_entity);
		registry.unloadables.emplace(pressure_plate_wall_entity, Unloadable{ position });
		Collidable& col = registry.collidables.emplace(pressure_plate_wall_entity);
		col.inelastic = true;

		const Animatable animatable = {
			ANIMATABLE_TYPE::PRESSURE_PLATE_WALL,
			ANIMATION_BEHAVIOR::LOOP,
			ANIMATION_TRIGGER::IDLE,
			1
		};
		registry.animatables.insert(pressure_plate_wall_entity, animatable);
		registry.textured.insert(pressure_plate_wall_entity, { TEXTURE_ASSET_ID::PRESSURE_PLATE_WALL });

		return pressure_plate_wall_entity;
}

Entity createMapTile(int tile_id, TEXTURE_ASSET_ID texture_id, ivec2 tile_position, vec4 dest_rect, vec4 source_rect, TILE_TYPE tile_type)
{
	Entity map_tile_entity;
	MapTile map_tile = { tile_id, tile_position, tile_type };
	registry.mapTiles.insert(map_tile_entity, map_tile);
	registry.positions.insert(map_tile_entity, { dest_rect.x, dest_rect.y });
	registry.scales.insert(map_tile_entity, { dest_rect.p, dest_rect.q });
	registry.angles.insert(map_tile_entity, { 0 });
	registry.renderSourceRects.insert(map_tile_entity, { vec2(source_rect.x, source_rect.y), vec2(source_rect.p, source_rect.q) });
	registry.textured.insert(map_tile_entity, { texture_id });
	registry.colors.insert(map_tile_entity, { DEFAULT_COLOUR_VEC4 });
	registry.renderRequests.insert(
		map_tile_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR
		}
	);

	return map_tile_entity;
}

Entity createTeleporter(const int level_uid, const int connected_level_id, vec2 position, vec2 size, vec2 connected_position, DIRECTION offset_direction)
{
	Entity teleporter_entity;
	registry.sizes.insert(teleporter_entity, { size.x, size.y });
	registry.positions.insert(teleporter_entity, position);
	Teleporter teleporter = { level_uid, connected_level_id, offset_direction, vec2(connected_position.x, connected_position.y) };
	registry.teleporters.emplace(teleporter_entity, teleporter);
	registry.collidables.emplace(teleporter_entity);
	registry.levelEntities.insert(teleporter_entity, { level_uid });

	return teleporter_entity;
}

Entity createWall(const int level_uid, vec2 position, float width, float height, vec4 color)
{
	const Entity& wall_rectangle_entity = createRectangle(position, width, height, RENDER_LAYER::WALLS_BACK, color);
	Collidable& col = registry.collidables.emplace(wall_rectangle_entity);
	col.inelastic = true;
	registry.walls.emplace(wall_rectangle_entity);
	registry.levelEntities.insert(wall_rectangle_entity, { level_uid });

	return wall_rectangle_entity;
}
