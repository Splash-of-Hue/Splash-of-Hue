#pragma once

#include "animation_components.hpp"
#include "animation_constants.hpp"
#include "ecs/entity.hpp"
#include <map>
#include <vector>

class AnimationSystem
{
private:
	std::map<ANIMATABLE_TYPE, std::map<ANIMATION_TRIGGER, unsigned int>> animation_entity_map;
	std::map<unsigned int, ANIMATION_TRIGGER> animation_trigger_queue_map;
	std::vector<Entity> animation_entities;
	int AnimationSystem::get_next_frame(Animatable animatable, int animation_frame_count);
	bool AnimationSystem::has_animation_entity(ANIMATABLE_TYPE type, ANIMATION_TRIGGER trigger);
	Entity AnimationSystem::get_animation_entity(ANIMATABLE_TYPE type, ANIMATION_TRIGGER trigger);
	unsigned int AnimationSystem::track_entity(Entity entity);
	void AnimationSystem::generate_animation(
		ANIMATABLE_TYPE type,
		ANIMATION_TRIGGER trigger,
		std::vector<std::pair<TEXTURE_ASSET_ID,
		RenderSourceRect>> texture_pairs,
		ANIMATION_TRIGGER end_trigger = ANIMATION_TRIGGER::ANIMATION_TRIGGER_COUNT
	);

public:
	void init();
	void step(float elapsed_ms);
	bool trigger_animation(Entity entity, ANIMATION_TRIGGER trigger);
};
