#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "save_components.hpp"
#include "save_constants.hpp"
#include "save_system.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

void SaveSystem::step(float elapsed_ms)
{
	if (registry.save_requests.size() == 0)
	{
		return;
	}

	save(SAVE_FILE_NAME);
	registry.save_requests.clear();
}

void SaveSystem::save(const std::string& filename)
{
	std::cout << "Saving level file: " << filename << std::endl;

	try
	{
		std::filesystem::path filename_path = std::filesystem::path(save_path(filename));
		std::cout << std::fixed;
		std::ifstream save_file_in(filename_path);
		std::ofstream save_file_out(filename_path);
		simdjson::json
		nlohmann::json loaded_json;
		save_file_in >> loaded_json;

		if (!loaded_json.contains("entities"))
		{
			loaded_json["entities"] = nlohmann::json({});
		}

		nlohmann::json& entities_json = loaded_json["entities"];

		for (Entity& save_entity : registry.savables.entities)
		{
			Savable save_data = registry.savables.get(save_entity);

			if (!entities_json.contains(save_data.id))
			{
				entities_json[save_data.id] = nlohmann::json({});
			}

			nlohmann::json& savable_json = entities_json[save_data.id];

			for (SAVE_TYPE& save_type : save_data.save_types)
			{
				switch (save_type)
				{
					case SAVE_TYPE::POSITION:
						vec2 position = registry.positions.get(save_entity);
						savable_json["position"] = { { "x", position.x }, { "y", position.y } };
						break;
					case SAVE_TYPE::REMOVED:
						//bool removed = registry.removed.has(save_entity);
						savable_json["removed"] = false;
						break;
				}
			}
		}

		// After making the changes, save the file
		save_file_out << loaded_json;
	}
	catch (...)
	{
		return;
	}

	return;
}
