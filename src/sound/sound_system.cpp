#include "sound_system.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include <cassert>
#include <sstream>
#include <iostream>
#include <fmod_studio.hpp>

SoundSystem::SoundSystem()
{
}

SoundSystem::~SoundSystem()
{
    fmodError(system->release());
    for (Entity& sound_entity : registry.sounds.entities)
    {
        Sound& sound = registry.sounds.get(sound_entity);
        fmodError(sound.sound->release());

        registry.remove_all_components_of(sound_entity);
    }
}

void SoundSystem::step(float elapsed_ms)
{
    Sound& backgroundSound = registry.sounds.get(backgroundEntity);
    //change isBackgroundMusicPlaying depending on if backgroundMusic has ended
    backgroundSound.channel->isPlaying(&backgroundSound.isSoundPlaying);
    //play backgroundMusic if it's stopped playing
    if (!backgroundSound.isSoundPlaying)
    {
        fmodError(system->playSound(backgroundSound.sound, nullptr, false, &backgroundSound.channel));
    }

    system->update();
}

void SoundSystem::init()
{
    Sound& backgroundSound = registry.sounds.emplace(backgroundEntity);
    backgroundSound.path = audio_path("Lofi Final.wav");
    //create system
    fmodError(FMOD::System_Create(&system));
    //initialize system
    system->init(32, FMOD_INIT_NORMAL, nullptr);
    //create background music
    fmodError(system->createSound(backgroundSound.path.c_str(), FMOD_DEFAULT, nullptr, &backgroundSound.sound));
    //play background music
    fmodError(system->playSound(backgroundSound.sound, nullptr, false, &backgroundSound.channel));
    backgroundSound.isSoundPlaying = true;
}

void SoundSystem::fmodError(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "Sound System Error!" << std::endl;
		exit(-1);
	}
}
