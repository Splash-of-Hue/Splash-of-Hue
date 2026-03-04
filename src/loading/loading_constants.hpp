#pragma once
#include <common.hpp>
#include <creature_system/creature_constants.hpp>
#include <ecs/components.hpp>
#include <map>
#include <string>
#include <interactables/interactable_constants.hpp>

// Currently just for loading rooms, but intentionally extendable
enum class LOAD_TYPE
{
	LOAD_LEVEL,
	UNLOAD_LEVEL,
};

enum class LOADING_LAYER_TYPE
{
	INVALID,
	CREATURES,
	FLOOR_INTERACTABLE,
	EXTRA_ENTITIES
};

enum class ENTITY_TYPE
{
	INVALID,
	TELEPORTER,
	INITIAL_POSITION,
	MOSS_TOGGLE,
	MOSS_WALL,
	WALL,
	COLOUR_ORB,
	BOX,
	CONTROL_PNG_ENTITY
};

const std::map<std::string, LOADING_LAYER_TYPE> layer_string_to_enum_map = {
	{ "Creatures", LOADING_LAYER_TYPE::CREATURES },
	{ "Floor_Interactables", LOADING_LAYER_TYPE::FLOOR_INTERACTABLE },
	{ "Extra_Entities", LOADING_LAYER_TYPE::EXTRA_ENTITIES },
};

inline LOADING_LAYER_TYPE layer_from_string(const std::string layer_string)
{
	const auto& search = layer_string_to_enum_map.find(layer_string);
	return search != layer_string_to_enum_map.end() ? search->second : LOADING_LAYER_TYPE::INVALID;
};

const std::map<std::string, ENTITY_TYPE> entity_type_string_to_enum_map = {
	{ "Teleporter", ENTITY_TYPE::TELEPORTER },
	{ "Initial_Position", ENTITY_TYPE::INITIAL_POSITION },
	{ "MossToggle", ENTITY_TYPE::MOSS_TOGGLE },
	{ "MossWall", ENTITY_TYPE::MOSS_WALL },
	{ "Wall", ENTITY_TYPE::WALL },
	{ "ColourOrb", ENTITY_TYPE::COLOUR_ORB },
	{ "Box", ENTITY_TYPE::BOX },
	{ "Control_PNG_entity", ENTITY_TYPE::CONTROL_PNG_ENTITY },
	// Control_PNG_entity
};

inline ENTITY_TYPE entity_type_from_string(const std::string entity_string)
{
	const auto& search = entity_type_string_to_enum_map.find(entity_string);
	return search != entity_type_string_to_enum_map.end() ? search->second : ENTITY_TYPE::INVALID;
};

const std::map<std::string, CREATURETYPE> creature_string_enum_map = {
	{ "Sawfish", CREATURETYPE::SAWFISH },
	{ "Anglerfish", CREATURETYPE::ANGLERFISH },
	{ "Crab", CREATURETYPE::CRAB },
	{ "Minnows", CREATURETYPE::MINNOWS },
	{ "PShrimp", CREATURETYPE::SHRIMP },
	{ "Whale", CREATURETYPE::WHALE },
};

inline CREATURETYPE creature_type_from_string(const std::string creature_string)
{
	const auto& search = creature_string_enum_map.find(creature_string);
	return search != creature_string_enum_map.end() ? search->second : CREATURETYPE::UNDEFINED;
};

const std::map<std::string, DIRECTION> direction_string_enum_map = {
	{ "Up", DIRECTION::UP },
	{ "Down", DIRECTION::DOWN },
	{ "Left", DIRECTION::LEFT },
	{ "Right", DIRECTION::RIGHT }
};

inline DIRECTION direction_type_from_string(const std::string direction_string)
{
	const auto& search = direction_string_enum_map.find(direction_string);
	return search != direction_string_enum_map.end() ? search->second : DIRECTION::UP; // defaults to UP
};

const std::map<std::string, CONTROL_PNG_TYPE> control_png_string_enum_map = {
	{ "Invalid", CONTROL_PNG_TYPE::INVALID },
	{ "WASD", CONTROL_PNG_TYPE::WASD },
	{ "ARROW_RED", CONTROL_PNG_TYPE::ARROW_RED },
	{ "ARROW_NO_RED", CONTROL_PNG_TYPE::ARROW_NO_RED },
	{ "COLOUR_AFFECTS", CONTROL_PNG_TYPE::COLOUR_AFFECTS}
};

inline CONTROL_PNG_TYPE control_png_type_from_string(const std::string control_png_string)
{
	const auto& search = control_png_string_enum_map.find(control_png_string);
	return search != control_png_string_enum_map.end() ? search->second : CONTROL_PNG_TYPE::INVALID;
};

const std::map<std::string, COLOUR_STATE> color_state_string_enum_map = {
	{ "Neutral", COLOUR_STATE::NEUTRAL },
	{ "Blue", COLOUR_STATE::BLUE },
	{ "Red", COLOUR_STATE::RED },
};

inline COLOUR_STATE color_state_from_string(const std::string color_string)
{
	const auto& search = color_state_string_enum_map.find(color_string);
	return search != color_state_string_enum_map.end() ? search->second : COLOUR_STATE::NEUTRAL;
};

// LDtk Constants
const std::string LDTK_PROJECT_PATH = asset_path() + "/ldtk/splashofhue.ldtk";
const std::string WORLD_ID_STRING = "53e64810-d380-11f0-a1bc-4d26a5cd15e3";
const std::string LEVEL_ID_STRING = "def30d80-d380-11f0-a1bc-2363b8ac7cbf";
