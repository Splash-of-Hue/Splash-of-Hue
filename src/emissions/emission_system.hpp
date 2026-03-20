#pragma once

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"

class EmissionSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);

private:
	void initialize_emission(Entity& emission_entity);
	void change_emission_colour_state(Entity& emission_entity, COLOUR_STATE colour_state);
	void remove_emission(Entity& emission_entity);
	void update_emission_size(Entity& emission_entity, float elapsed_ms);
	void update_emission_position(Entity& emission_entity);
};
