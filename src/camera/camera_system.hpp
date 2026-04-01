#pragma once
#include "ecs/system.hpp"
#include "camera_constants.hpp"

class CameraSystem : public SystemInterface
{
public:
	// This checks if there are any ChangeCameraTargetRequest
	void step(float elapsed_ms);
private:
	// TODO BbJdP3lK: Remove, M4 hack, add 1 for safety  so we are above the cooldown and it isn't triggered right away
	float loading_screen_timer = LOADING_SCREEN_CAMERA_COOLDOWN + 1;
};
