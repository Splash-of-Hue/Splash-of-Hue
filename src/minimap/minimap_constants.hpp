#pragma once
#include "common.hpp"

const float MINIMAP_PADDING = 14.f;
const float MINIMAP_ANCHOR_X = WINDOW_WIDTH_PX - MINIMAP_PADDING;
const float MINIMAP_ANCHOR_Y = MINIMAP_PADDING;
const float MINIMAP_MAX_SIZE = 200.f;
const float MINIMAP_ICON_SIZE = 10.f;

const vec4 MINIMAP_COLOR_VISITED = { 0.55f, 0.80f, 0.92f, 0.70f };
const vec4 MINIMAP_COLOR_PLAYER = { 0.85f, 0.41f, 0.84f, 1.00f };