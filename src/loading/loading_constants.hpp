#pragma once
#include <common.hpp>
#include <creature_behaviour_system/creature_constants.hpp>
#include <ecs/components.hpp>
#include <map>
#include <string>

// Currently just for loading rooms, but intentionally extendable
enum class LOAD_TYPE
{
	LOAD_LEVEL,
	UNLOAD_LEVEL,
};

enum class LOADING_LAYER_TYPE
{
	INVALID,
	CREATURES
};

const std::map<std::string, LOADING_LAYER_TYPE> layer_string_to_enum_map = {
	{ "Creatures", LOADING_LAYER_TYPE::CREATURES },
};
inline LOADING_LAYER_TYPE layer_from_string(const std::string layer_string)
{
	const auto& search = layer_string_to_enum_map.find(layer_string);
	return search != layer_string_to_enum_map.end() ? search->second : LOADING_LAYER_TYPE::INVALID;
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
