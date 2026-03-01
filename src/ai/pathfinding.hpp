#pragma once
#include "common.hpp"
#include <ecs/entity.hpp>
#include <unordered_map>
#include <vector>

bool direct_line_next_step(float elapsed_ms, Entity& follower);
std::vector<ivec2> a_star(ivec2 start_position_grid, ivec2 end_position_grid, std::unordered_map<ivec2, bool> valid_grid_coordinates);
