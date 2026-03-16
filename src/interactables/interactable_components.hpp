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

enum class MUSHROOM_SIZE 
{
	SMALL = 0,
	MEDIUM = SMALL + 1,
	LARGE = MEDIUM + 1
};

struct Mushroom
{
	MUSHROOM_SIZE size = DEFAULT_MUSHROOM_SIZE;
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
