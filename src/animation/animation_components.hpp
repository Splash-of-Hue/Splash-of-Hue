#pragma once
#include "animation_constants.hpp"
#include <renderer/render_constants.hpp>
#include <vector>

/*
This is the current animation, since mutiple of the same things could be on the screen,
only want to know the current animation for each of them independent of each other.
Everything that has an animation has this component.
*/
struct Animatable
{
	ANIMATABLE_TYPE type = ANIMATABLE_TYPE::ANIMATABLE_TYPE_COUNT;
	ANIMATION_BEHAVIOR behavior = ANIMATION_BEHAVIOR::ANIMATION_BEHAVIOR_COUNT;
	ANIMATION_TRIGGER current_trigger = ANIMATION_TRIGGER::ANIMATION_TRIGGER_COUNT;
	// The rate at which to play the animation, high is faster
	int frame_rate = 30;
	// The index of the current frame from the Animation
	int current_frame = 0;
	// The amount of time the current frame has been rendered
	float time_on_current_frame = 0;
	// If the animatable is finished and should move on
	bool finished = false;
	// If this is false, the animation finishes before switching to the next animation
	bool cancellable = true;
};

/*
CAUTION: This should not be associated with the entity being animated.
This is associated with a unique entity which is known by Animatable.
This animation will be reused for everything of the same type doing the same thing.
*/
struct Animation
{
	// A list of texture IDs in order
	std::vector<TEXTURE_ASSET_ID> texture_ids;
	// This is the trigger for the next animation, if the behavior is to only play once
	ANIMATION_TRIGGER next_animation_trigger;
};

// This is so any system can trigger an action for an entity
struct AnimationTrigger
{
	ANIMATION_TRIGGER animation_trigger;
};
