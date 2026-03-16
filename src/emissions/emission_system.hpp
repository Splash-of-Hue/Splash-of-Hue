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
	void remove_emissions();
	void create_new_emissions();
	void change_emission_colour();
	void update_emission_position_size(float elapsed_ms);
};
