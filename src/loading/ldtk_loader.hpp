#pragma once
#include <LDtkLoader/Entity.hpp>
#include <LDtkLoader/Layer.hpp>
#include <LDtkLoader/Project.hpp>
#include "loading_constants.hpp"

static bool project_loaded = false;
static ldtk::Project ldtk_project;
const auto& LDTK_WORLD_ID = ldtk::IID::IID(WORLD_ID_STRING);
bool is_initial_load = true;
vec2 LEVEL_TO_SCREEN_SCALE;
float LDTK_CAMERA_ROOM_SPEED = 800.f;

void load_tile_layer(const ldtk::Layer& layer);
void load_creatures(const ldtk::Layer& layer);
void load_spawn_points(const ldtk::Entity& spawn_point_entity);
void load_floor_interactables(const ldtk::Layer& layer);
void load_entity_layer(const ldtk::Layer& layer);
void load_extra_entities(const ldtk::Layer& layer);
void use_initial_player_position(const ldtk::Entity& initial_player_position_entity);
