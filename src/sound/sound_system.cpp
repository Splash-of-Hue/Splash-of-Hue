#include "sound_system.hpp"
#include <cassert>
#include <sstream>
#include <iostream>
SoundSystem::SoundSystem()
{
}

SoundSystem::~SoundSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (invader_hit_sound != nullptr)
		Mix_FreeChunk(invader_hit_sound);
	if (tower_hit_sound != nullptr)
		Mix_FreeChunk(tower_hit_sound);
	Mix_CloseAudio();

}

bool SoundSystem::start_and_load_sounds() {
	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	background_music = Mix_LoadMUS(audio_path("Lowdown.wav").c_str());
	//invader_hit_sound = Mix_LoadWAV(audio_path("invader_hit.wav").c_str());
	//tower_hit_sound = Mix_LoadWAV(audio_path("tower_hit.wav").c_str());

	if (background_music == nullptr || invader_hit_sound == nullptr || tower_hit_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("invader_hit.wav").c_str(),
			audio_path("tower_hit.wav").c_str());
		return false;
	}

	return true;
}


void SoundSystem::init() {
	// start playing background music indefinitely
	std::cout << "Starting music..." << std::endl;
	Mix_PlayMusic(background_music, -1);
}