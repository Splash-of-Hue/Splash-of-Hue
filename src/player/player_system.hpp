#pragma once
#include "common.hpp"
#include "ecs/system.hpp"

class PlayerSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
private:
	void create_player(int level_uid, vec2 position);
};
