#pragma once
#include "ecs/system.hpp"

class CameraSystem : public SystemInterface
{
public:
	// This checks if there are any ChangeCameraTargetRequest
	void step(float elapsed_ms);
};
