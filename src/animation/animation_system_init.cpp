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
		{
				{ TEXTURE_ASSET_ID::PLAYER_TEXTURE_01, RenderSourceRect() }
		}
	);

	/*
		CREATURE ANIMATIONS
	*/
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::SAWFISH_01, RenderSourceRect() }
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::WHALE,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::WHALE_01, RenderSourceRect() }
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::PISTOL_SHRIMP,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect() }
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::MINNOW,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::MINNOW_01, RenderSourceRect() }
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::JAPANESE_PORCUPINE_CRAB,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::CREATURES, RenderSourceRect { vec2(0), vec2(0.125) }},
				{ TEXTURE_ASSET_ID::CREATURES, RenderSourceRect { vec2(0.125, 0), vec2(0.125) }}
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::ANGLERFISH,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::ANGLERFISH_01, RenderSourceRect() }
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::CORAL_TREE,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::CORAL_TREE_01, RenderSourceRect() }
		}
	);
}

void AnimationSystem::generate_animation(
	ANIMATABLE_TYPE type,
	ANIMATION_TRIGGER trigger,
	std::vector<std::pair<TEXTURE_ASSET_ID, RenderSourceRect>> texture_pairs,
	ANIMATION_TRIGGER end_trigger
)
{
	Entity entity;
	animation_entity_map[type][trigger] = track_entity(entity);
	std::vector<TEXTURE_ASSET_ID> textures;
	std::vector<RenderSourceRect> texture_source_rects;

	for (auto& texture_pair : texture_pairs)
	{
		textures.push_back(texture_pair.first);
		texture_source_rects.push_back(texture_pair.second);
	}

	Animation animation = { textures, end_trigger, texture_source_rects };
	registry.animations.emplace(entity, animation);
}

unsigned int AnimationSystem::track_entity(Entity entity)
{
	animation_entities.push_back(entity);
	return (int)animation_entities.size() - 1;
}
