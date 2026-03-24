#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "common.hpp"
#include <fmod_studio.hpp>

class SoundSystem
{
public:

	SoundSystem();

	~SoundSystem();

	bool start_and_load_sounds();

	void init();

private:
	// music references
	Mix_Music* background_music;

	//Mix_Chunk* invader_hit_sound;

	//Mix_Chunk* tower_hit_sound;
};
