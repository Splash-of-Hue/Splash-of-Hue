#pragma once

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
	Entity background_entity;
	FMOD_RESULT result;

	void validate_fmod_result(FMOD_RESULT result);
};
