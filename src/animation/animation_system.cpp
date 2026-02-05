#include "animation_components.hpp"
#include "animation_constants.hpp"
#include "animation_system.hpp"
#include "ecs/registry.hpp"
#include "renderer/render_constants.hpp"
#include <iostream>

void AnimationSystem::step(float elapsed_ms)
{
	auto& animatable_registry = registry.animatables;
	auto& animation_trigger_registry = registry.animation_triggers;
	auto& textured_registry = registry.textured;
	auto& render_request_registry = registry.renderRequests;

	for (Entity entity : animation_trigger_registry.entities)
	{
		// For each active trigger, queue it
		auto& c = animation_trigger_registry.get(entity);
		trigger_animation(entity, c.animation_trigger);
	}

	// Clear triggers each frame after they have been queued
	animation_trigger_registry.clear();

	for (Entity entity : animatable_registry.entities)
	{
		// If there is nothing requested to be rendered, simply move on
		if (!render_request_registry.has(entity))
		{
			continue;
		}

		auto& animatable = animatable_registry.get(entity);
		animatable.time_on_current_frame += elapsed_ms;

		// If we should remain on the current frame, we are done
		if (animatable.time_on_current_frame < ms_per_frame(animatable.frame_rate))
		{
			continue;
		}

		if (!has_animation_entity(animatable.type, animatable.current_trigger))
		{
			std::cout << "ERROR: Failed to trigger animation for an animatable type: " << (int)animatable.type << " with trigger ID: " << (int)animatable.current_trigger << std::endl;
			continue;
		}

		// We now want to update to the next frame with the current animation and know there is a render request
		auto animation_entity = get_animation_entity(animatable.type, animatable.current_trigger);
		auto& animation = registry.animations.get(animation_entity);
		auto& animation_texture_ids = animation.texture_ids;

		// If this should only play once, check if it should trigger the next animation and then keep going if it does
		if (animatable.finished && animatable.behavior == ANIMATION_BEHAVIOR::PLAY_ONCE)
		{
			// First check if there is an animation queued, and if so, use that, otherwise, there should be one to return to

			if (animation_trigger_queue_map.count(entity) > 0)
			{
				trigger_animation(entity, animation_trigger_queue_map[entity]);
			}
			else if ((int)animation.next_animation_trigger > -1)
			{
				// Only do this if there is a next animation to be triggered
				trigger_animation(entity, animation.next_animation_trigger);
			}

			continue;
		}

		// The next frame will depend on the behavior, use a helper function
		int next_frame = get_next_frame(animatable, (int)animation_texture_ids.size());
		TEXTURE_ASSET_ID texture_asset_id = animation_texture_ids[next_frame];
		auto& textured = textured_registry.get(entity);
		// Update both components
		textured.used_texture = texture_asset_id;
		animatable.current_frame = next_frame;
		animatable.time_on_current_frame = 0;
		//std::cout << "ANIMATION: Using texture  ID: " << (int)texture_asset_id << " for entity ID: " << (int)entity.id() << ", time spent on this frame: " << animatable.time_on_current_frame << std::endl;
	}

	return;
}

bool AnimationSystem::trigger_animation(Entity entity, ANIMATION_TRIGGER trigger)
{
	auto& animatable_registry = registry.animatables;

	// If this entity isn't animatable, ignore but print
	if (!animatable_registry.has(entity))
	{
		std::cout << "WARN: Failed to trigger animation for entity: " << entity.id() << "with trigger ID: " << (int)trigger << std::endl;
		return false;
	}

	auto& animatable = animatable_registry.get(entity);

	if (!has_animation_entity(animatable.type, animatable.current_trigger))
	{
		std::cout << "ERROR: Failed to trigger animation for an animatable type: " << (int)animatable.type << "with trigger ID: " << (int)animatable.current_trigger << std::endl;
		return false;
	}

	if (!animatable.cancellable && !animatable.finished)
	{
		// We have to wait until the current animation finishes, but queue up to action
		animation_trigger_queue_map[entity] = trigger;
		return true;
	}

	animatable.current_trigger = trigger;

	return true;
}

int AnimationSystem::get_next_frame(Animatable animatable, int animation_frame_count)
{
	int next_frame = 0;

	switch (animatable.behavior)
	{
		case ANIMATION_BEHAVIOR::LOOP:
			next_frame = (animatable.current_frame + 1) % animation_frame_count;
			break;
		case ANIMATION_BEHAVIOR::PING_PONG:
			if (animatable.current_frame == 0 && animatable.finished)
			{
				animatable.finished = false;
			}

			if (animatable.current_frame == animation_frame_count - 1 && !animatable.finished)
			{
				animatable.finished = true;
			}

			next_frame = animatable.current_frame + (animatable.finished ? -1 : 1);

			break;
		case ANIMATION_BEHAVIOR::PLAY_ONCE:
			if (animatable.finished || animatable.current_frame == animation_frame_count - 1)
			{
				animatable.finished = true;
				return animatable.current_frame;
			}

			next_frame = animatable.current_frame + 1;

			// Don't need to break, since it's on bottom
	}

	return next_frame;
}

// Used for initializing new animatables. Should not be used much outside the AnimationSystem class
bool AnimationSystem::has_animation_entity(ANIMATABLE_TYPE type, ANIMATION_TRIGGER trigger)
{
	if (animation_entity_map.count(type) < 1)
	{
		return false;
	}

	if (animation_entity_map[type].count(trigger) < 1)
	{
		return false;
	}

	return true;
}

/*
	CAUTION: Do not call unless you confirm this exists with has_animation_entity
*/
Entity AnimationSystem::get_animation_entity(ANIMATABLE_TYPE type, ANIMATION_TRIGGER trigger)
{
	auto animation_entity_index = animation_entity_map[type][trigger];
	return animation_entities[animation_entity_index];
}
