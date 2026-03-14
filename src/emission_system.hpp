#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"

class EmissionSystem
{
public:

	void step(float elapsed_ms);
	EmissionSystem()
	{
	}

private:
	void removeEmissions();
	void createNewEmissions();
	void changeDifferentEmissions();
	void updateCurrentEmissions(float elapsed_ms);

};
