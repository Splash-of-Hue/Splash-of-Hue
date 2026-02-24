#pragma once

#include "common.hpp"

// If an entity is to be rendered, the camera will move it, unless it has this component.
// This should be used for things like UI.
struct IgnoresCamera {};

struct Camera {};

struct ChangeCameraTargetRequest
{
	vec2 target;
	float speed;
};
