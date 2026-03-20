#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/system.hpp"
#include "ecs/tiny_ecs.hpp"

class InteractableSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
private:
	void step_moss_toggle();
	void step_pressure_plates();
	void step_colour_orbs();
	void step_mushrooms();
	void handle_mushrooms(Entity& mushroom_entity);
	bool has_valid_emission(std::vector<Entity>& radiusList);
	COLOUR_STATE change_colour_given_priority(std::multimap<bool, std::vector<Entity>> radiiList);
};
