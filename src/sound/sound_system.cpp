#include "sound_system.hpp"
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
    fmodError(backgroundMusic->release());
}

void SoundSystem::step(float elapsed_ms)
{
    //change isBackgroundMusicPlaying depending on if backgroundMusic has ended
    backgroundMusicChannel->isPlaying(&isBackgroundMusicPlaying);
    //play backgroundMusic if it's stopped playing
    if (!isBackgroundMusicPlaying)
    {
        fmodError(system->playSound(backgroundMusic, nullptr, false, &backgroundMusicChannel));
    }

    system->update();
}

void SoundSystem::init()
{
    //create system
    fmodError(FMOD::System_Create(&system));
    //initialize system
    system->init(32, FMOD_INIT_NORMAL, nullptr);
    //create background music
    fmodError(system->createSound(backgroundMusicString, FMOD_DEFAULT, nullptr, &backgroundMusic));
    //play background music
    fmodError(system->playSound(backgroundMusic, nullptr, false, &backgroundMusicChannel));
    isBackgroundMusicPlaying = true;
}

void SoundSystem::fmodError(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "Sound System Error!" << std::endl;
		exit(-1);
	}
}
