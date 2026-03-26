#pragma once
#include "ecs/system.hpp"
#include <string>

class SaveSystem : public SystemInterface
{
private:
	void save(const std::string& filename);

public:
	// This checks if there are any LoadRequests and if
	// there are it processes them with the ID.
	void step(float elapsed_ms);
};
