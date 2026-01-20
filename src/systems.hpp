#pragma once

// external function definitions for ECS systems
#include "tinyECS/registry.hpp"

// forward function declaractions (ordered alphabetically)
int invader_detection_system(Registry& entity_registry);
int invader_movement_system(Registry& entity_registry, float max_x);
int invader_release_system(Registry& entity_registry);
