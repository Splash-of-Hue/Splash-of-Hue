#include "sound_system.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include <cassert>
#include <sstream>
#include <iostream>
#include <fmod_studio.hpp>
#include <fmod_errors.h>

SoundSystem::SoundSystem()
{
}

SoundSystem::~SoundSystem()
{
    for (Entity& sound_entity : registry.sounds.entities)
    {
        Sound& sound = registry.sounds.get(sound_entity);
        validate_fmod_result(sound.sound->release());

        registry.remove_all_components_of(sound_entity);
    }

    validate_fmod_result(system->release());
}

void SoundSystem::step(float elapsed_ms)
{
    Sound& background_sound = registry.sounds.get(background_entity);
    //change isBackgroundMusicPlaying depending on if backgroundMusic has ended
    background_sound.channel->isPlaying(&background_sound.isSoundPlaying);

    //play backgroundMusic if it's stopped playing
    if (!background_sound.isSoundPlaying)
    {
        validate_fmod_result(system->playSound(background_sound.sound, nullptr, false, &background_sound.channel));
    }

    system->update();
}

void SoundSystem::init()
{
    Sound& background_sound = registry.sounds.emplace(background_entity);
    background_sound.path = audio_path("Lofi Final.wav");
    //create system
    validate_fmod_result(FMOD::System_Create(&system));
    //initialize system
    system->init(32, FMOD_INIT_NORMAL, nullptr);
    //create background music
    validate_fmod_result(system->createSound(background_sound.path.c_str(), FMOD_DEFAULT, nullptr, &background_sound.sound));
    //play background music
    validate_fmod_result(system->playSound(background_sound.sound, nullptr, false, &background_sound.channel));
    background_sound.isSoundPlaying = true;
}

void SoundSystem::validate_fmod_result(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "Sound System Error: " << FMOD_ErrorString(result) <<  std::endl;
		exit(-1);
	}
}
