#pragma once

#include "ecs/components.hpp"
#include "interactables/interactable_constants.hpp"
#include <functional>
#include <string>
#include <vector>

struct Interactable
{
	INTERACTABLE_TYPE type = INTERACTABLE_TYPE::INVALID;
};

struct Box
{};

struct ColourOrb
{
	COLOUR_STATE colour = COLOUR_STATE::NEUTRAL;
};

struct OrbCollected
{};

struct Mushroom
{
	MUSHROOM_SIZE size = DEFAULT_MUSHROOM_SIZE;
};

struct PuzzleBuildRequest
{
	Entity puzzle_entity;
	std::vector<std::string> unloaded_puzzle_piece_ids;
	std::vector<std::string> unloaded_puzzle_trigger_ids;
};

struct Puzzle
{
	std::vector<Entity> puzzle_pieces;
	std::vector<Entity> puzzle_triggers;
};

struct PuzzleTrigger
{
	std::string id;
	std::function<void()> callback;
};

struct PuzzlePiece
{
	std::string id;
	bool is_correct = false;
};

struct PuzzleBubble
{
	COLOUR_STATE solution_colour;
};

struct MossToggle
{
	std::vector<std::string> connected_wall_ids;
};

/** General components **/

// for things that break on contact (i.e. boxes)
struct Breakable
{};

// for things that are collectable (i.e. colour orb by the player)
struct Collectable
{};
