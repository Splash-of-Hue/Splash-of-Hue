#pragma once
#include "loading_constants.hpp"
#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Project.hpp>

static bool project_loaded = false;
static ldtk::Project ldtk_project;
const auto& LDTK_WORLD_ID = ldtk::IID::IID(WORLD_ID_STRING);
bool is_initial_load = true;
vec2 LEVEL_TO_SCREEN_SCALE;
float LDTK_CAMERA_ROOM_SPEED = 1000.f;

// Tiles
void load_tile_layer(const ldtk::Layer& layer);
// Entities
void load_entity_layer(const ldtk::Layer& layer);
// Entities - Creatures
void load_creatures(const ldtk::Layer& layer);
// Entities - Floor interactables
void load_floor_interactables(const ldtk::Layer& layer);
void load_teleporter(const ldtk::Entity& teleporter_entity);
void load_moss_toggle(const ldtk::Entity& moss_toggle_entity);
void load_moss_door(const ldtk::Entity& moss_door_entity);
void load_pressure_plate(const ldtk::Entity& pressure_plate_entity);
void load_pressure_plate_walls(const ldtk::Entity& pressure_plate_entity);
void load_colour_orb(const ldtk::Entity& colour_orb_entity);
void load_box(const ldtk::Entity& box_entity);
void load_mushroom(const ldtk::Entity& mushroom_entity);
void load_puzzle_bubble(const ldtk::Entity& puzzle_bubble_entity);
// Entities - Extras
void load_extra_entities(const ldtk::Layer& layer);
void initialize_player(const ldtk::Entity& initial_player_position_entity);
void load_wall(const ldtk::Entity& wall_entity);
void load_minimap_reference(const ldtk::Entity& minimap_reference_entity);
void load_minimap_hallway(const ldtk::Entity& minimap_hallway_entity);
void load_puzzle_wall(const ldtk::Entity& puzzle_wall_entity);
void load_puzzle_solution(const ldtk::Entity& puzzle_solution_entity);
// Entities - Decorations
void load_decorations(const ldtk::Layer& layer);
void load_puzzle_bubble_fake(const ldtk::Entity& puzzle_bubble_fake_entity);
// Entities - Minimap
void laod_minimap(const ldtk::Layer& layer);
// tutorial pics
void load_control_pictures(const ldtk::Entity& control_picture_entity);
void load_floor_hint_pictures(const ldtk::Entity& floor_hint_png_entity);

const inline vec2 scale_to_level(vec2 input)
{
	return input * LEVEL_TO_SCREEN_SCALE;
}
