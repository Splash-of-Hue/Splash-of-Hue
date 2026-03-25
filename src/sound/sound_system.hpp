#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "common.hpp"
#include <fmod_studio.hpp>
#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"

class SoundSystem : public SystemInterface
{
public:

	SoundSystem();

	~SoundSystem();

	void init();

	void step(float elapsed_ms);

private:
	FMOD::System* system = nullptr;

	FMOD::Channel* backgroundMusicChannel = nullptr;
	FMOD::Sound* backgroundMusic = nullptr;
	bool isBackgroundMusicPlaying = false;
	std::string backgroundMusicString = audio_path("Lofi Final.wav");

	void fmodError(FMOD_RESULT result);
};
