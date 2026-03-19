#pragma once
#include "ecs/system.hpp"

class MenuControllerSystem : public SystemInterface
{
public:
	void step(float elapsed_ms);
private:
	const float MENU_MOVE_STOCK_DEADZONE = 0.2;
};
