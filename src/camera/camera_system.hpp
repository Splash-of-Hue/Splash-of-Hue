#pragma once
#include "ecs/system.hpp"
#include "camera_constants.hpp"

class CameraSystem : public SystemInterface
{
public:
	// This checks if there are any ChangeCameraTargetRequest
	void step(float elapsed_ms);
private:
	// TODO: Remove, M4 hack
	float loading_screen_timer = LOADING_SCREEN_CAMERA_COOLDOWN + 1;
};
