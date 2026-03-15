#pragma once
#include "common.hpp"

// Player component
struct Player
{
	float speed = PLAYER_SPEED;
	bool is_loaded = false;
	bool blue_unlocked = false;
	bool red_unlocked = false;
};

struct PlayerCreateRequest
{
	int level_uid;
	vec2 starting_position;
};
