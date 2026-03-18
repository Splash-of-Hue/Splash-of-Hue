#pragma once

#include "interactables/interactable_constants.hpp"
#include "ecs/components.hpp"
#include <string>
#include <vector>

struct Interactable
{
	INTERACTABLE_TYPE type = INTERACTABLE_TYPE::INVALID;
};

struct Box
{
};

struct ColourOrb
{
	COLOUR_STATE colour = COLOUR_STATE::NEUTRAL;
};

struct OrbCollected {
};

struct Mushroom
{
	MUSHROOM_SIZE size = default_mushroom_size;
};

struct MossToggle
{
	std::vector<std::string> connected_wall_ids;
};

/** General components **/

// for things that break on contact (i.e. boxes)
struct Breakable
{
};

// for things that are collectable (i.e. colour orb by the player)
struct Collectable
{
};
