#include "animation_components.hpp"
#include "animation_constants.hpp"
#include "animation_system.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "renderer/render_constants.hpp"
#include <vector>

void AnimationSystem::init()
{
	animation_entities = {};
	animation_trigger_queue_map = {};
	animation_entity_map = {};

	/*
		PLAYER ANIMATIONS
	*/
	generate_animation(
		ANIMATABLE_TYPE::PLAYER,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::PLAYER_TEXTURE_01 }
	);

	/*
		CREATURE ANIMATIONS
	*/
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::SAWFISH_01 }
	);
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::SAWFISH_01 }
	);
	generate_animation(
		ANIMATABLE_TYPE::PISTOL_SHRIMP,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01 }
	);
	generate_animation(
		ANIMATABLE_TYPE::MINNOW,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::MINNOW_01 }
	);
	generate_animation(
		ANIMATABLE_TYPE::JAPANESE_PORCUPINE_CRAB,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::JAPANESE_PORCUPINE_CRAB_01 }
	);
	generate_animation(
		ANIMATABLE_TYPE::ANGLERFISH,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::ANGLERFISH_01 }
	);
	generate_animation(
		ANIMATABLE_TYPE::CORAL_TREE,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::CORAL_TREE_01 }
	);
}

void AnimationSystem::generate_animation(
	ANIMATABLE_TYPE type,
	ANIMATION_TRIGGER trigger,
	std::vector<TEXTURE_ASSET_ID> textures,
	ANIMATION_TRIGGER end_trigger
)
{
	Entity entity;
	animation_entity_map[type][trigger] = track_entity(entity);
	Animation animation = { textures, end_trigger };
	registry.animations.emplace(entity, animation);
}

unsigned int AnimationSystem::track_entity(Entity entity)
{
	animation_entities.push_back(entity);
	return (int)animation_entities.size() - 1;
}
