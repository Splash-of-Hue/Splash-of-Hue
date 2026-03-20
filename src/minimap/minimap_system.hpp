#pragma once
#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"

class MinimapSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);

private:
	Entity create_room_entity();
	Entity player_icon_entity;
	bool player_icon_created = false;

};
