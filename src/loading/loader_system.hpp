#pragma once
#include "ecs/registry.hpp"
#include "ecs/system.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <string>

class LoaderSystem : public SystemInterface
{
private:
	nlohmann::json loaded_data;
	void load_file(const std::string& filename);
	void load_level(int level_uid);
	void unload_level(int level_uid);

public:
	// This checks if there are any LoadRequests and if
	// there are it processes them with the ID.
	void step(float elapsed_ms);
};
