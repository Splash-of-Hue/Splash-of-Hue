#include "sound_system.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "sound_constants.hpp"
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
        result = sound.sound->release();
        validate_fmod_result(result);

        registry.remove_all_components_of(sound_entity);
    }

    result = system->release();
    validate_fmod_result(result);
}

void SoundSystem::step(float elapsed_ms)
{
    Sound& background_sound = registry.sounds.get(background_entity);
    //change isBackgroundMusicPlaying depending on if backgroundMusic has ended
    background_sound.channel->isPlaying(&background_sound.isSoundPlaying);

    //play backgroundMusic if it's stopped playing
    if (!background_sound.isSoundPlaying)
    {
        //play background music
        result = system->playSound(background_sound.sound, nullptr, false, &background_sound.channel);
        validate_fmod_result(result);
        //add offset to background music
        result = background_sound.channel->setPosition(LOOP_TIME, FMOD_TIMEUNIT_MS);
        validate_fmod_result(result);
    }

    system->update();
}

void SoundSystem::init()
{
    Sound& background_sound = registry.sounds.emplace(background_entity);
    background_sound.path = audio_path("New Lofi Final.wav");
    //create system
    result = FMOD::System_Create(&system);
    validate_fmod_result(result);
    //initialize system
    result = system->init(32, FMOD_INIT_NORMAL, nullptr);
    validate_fmod_result(result);
    //create background music
    result = system->createSound(background_sound.path.c_str(), FMOD_DEFAULT, nullptr, &background_sound.sound);
    validate_fmod_result(result);
    //play background music
    result = system->playSound(background_sound.sound, nullptr, false, &background_sound.channel);
    validate_fmod_result(result);

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
