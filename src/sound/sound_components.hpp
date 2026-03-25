#pragma once

#include "ecs/components.hpp"
#include "sound_system.hpp"
#include <functional>
#include <string>
#include <vector>


struct Sound
{
	FMOD::Channel* channel = nullptr;
	FMOD::Sound* sound = nullptr;
	bool isSoundPlaying;
	std::string path;
};
