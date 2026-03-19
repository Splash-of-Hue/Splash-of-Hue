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
	void handle_mushrooms(Entity& mushroom_entity);
	COLOUR_STATE change_colour_given_priority(bool& inRadius1, std::vector<Entity>& radiusList1, bool& inRadius2, std::vector<Entity>& radiusList2, bool& inRadius3, std::vector<Entity>& radiusList3);
};
