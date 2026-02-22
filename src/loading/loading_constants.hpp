#pragma once
#include <common.hpp>
#include <string>

// Currently just for loading rooms, but intentionally extendable
enum class LOAD_TYPE
{
	LOAD_LEVEL,
	UNLOAD_LEVEL,
};

// LDtk Constants
const std::string LDTK_PROJECT_PATH = asset_path() + "/ldtk/splashofhue.ldtk";
const std::string WORLD_ID_STRING = "53e64810-d380-11f0-a1bc-4d26a5cd15e3";
const std::string LEVEL_ID_STRING = "def30d80-d380-11f0-a1bc-2363b8ac7cbf";
