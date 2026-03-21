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
	// Idle - frames 0 and 1 on spritesheet
	generate_animation(
		ANIMATABLE_TYPE::PLAYER,
		ANIMATION_TRIGGER::IDLE,
		{
			{ TEXTURE_ASSET_ID::PLAYER_TEXTURE_01, RenderSourceRect { vec2(0.0f, 0.0f), vec2(0.2f, 1.0f) }},
			{ TEXTURE_ASSET_ID::PLAYER_TEXTURE_01, RenderSourceRect { vec2(0.2f, 0.0f), vec2(0.2f, 1.0f) }}
		}
	);

	// Walk -> frames 2, 3, 4 on spritesheet
	generate_animation(
		ANIMATABLE_TYPE::PLAYER,
		ANIMATION_TRIGGER::WALK,
		{
			{ TEXTURE_ASSET_ID::PLAYER_TEXTURE_01, RenderSourceRect { vec2(0.4f, 0.0f), vec2(0.2f, 1.0f) }},
			{ TEXTURE_ASSET_ID::PLAYER_TEXTURE_01, RenderSourceRect { vec2(0.6f, 0.0f), vec2(0.2f, 1.0f) }},
			{ TEXTURE_ASSET_ID::PLAYER_TEXTURE_01, RenderSourceRect { vec2(0.8f, 0.0f), vec2(0.2f, 1.0f) }}
		}
	);

	/*
		CREATURE ANIMATIONS
	*/
	// Sawfish idle, just one frame, not moving
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::IDLE,
		{
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.25f, 0.0f), vec2(0.25f, 0.25f) } },
		}
	);

	// Sawfish swimming, tail flapping
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::WALK,
		{
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.25f, 0.0f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.0f, 0.0f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.25f, 0.0f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.5f, 0.0f), vec2(0.25f, 0.25f) } },
		}
	);

	// Sawfish happy eyes for following while blue
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::HAPPY,
		{
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.75f, 0.0f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.0f, 0.25f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.75f, 0.0f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.25f, 0.25f), vec2(0.25f, 0.25f) } }
		}
	);

	// Sawfish angry eyes for following while red
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::ANGRY,
		{
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.5f, 0.25f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.75f, 0.25f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.5f, 0.25f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.0f, 0.5f), vec2(0.25f, 0.25f) } }
		}
	);

	// Sawfish charging is just one frame
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::CHARGE,
		{
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.25f, 0.5f), vec2(0.25f, 0.25f) } },
		}
	);

	// Sawfish charges and bounces back, then returns to idle
	generate_animation(
		ANIMATABLE_TYPE::SAWFISH,
		ANIMATION_TRIGGER::CRASH,
		{
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.5f, 0.5f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.75f, 0.5f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.0f, 0.75f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.25f, 0.75f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.0f, 0.75f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.75f, 0.5f), vec2(0.25f, 0.25f) } },
			{ TEXTURE_ASSET_ID::SAWFISH, RenderSourceRect { vec2(0.5f, 0.5f), vec2(0.25f, 0.25f) } },
		},
		ANIMATION_TRIGGER::IDLE
	);

	generate_animation(
		ANIMATABLE_TYPE::WHALE,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::WHALE_01, RenderSourceRect() }
		}
	);

	/** DOWN P Shrimp **/
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_DOWN, ANIMATION_TRIGGER::SHRIMP_BLUE, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.0f,   0.0f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.1667f,0.0f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_DOWN, ANIMATION_TRIGGER::SHRIMP_NEUTRAL, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.3333f,0.0f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.5f,   0.0f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_DOWN, ANIMATION_TRIGGER::SHRIMP_RED, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.6667f,0.0f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.8333f,0.0f), vec2(0.1667f, 0.25f) }}
		});

	/** LEFT P Shrimp **/
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_LEFT, ANIMATION_TRIGGER::SHRIMP_BLUE, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.0f,   0.25f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.1667f,0.25f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_LEFT, ANIMATION_TRIGGER::SHRIMP_NEUTRAL, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.3333f,0.25f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.5f,   0.25f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_LEFT, ANIMATION_TRIGGER::SHRIMP_RED, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.6667f,0.25f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.8333f,0.25f), vec2(0.1667f, 0.25f) }}
		});

	/** RIGHT P Shrimp **/
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_RIGHT, ANIMATION_TRIGGER::SHRIMP_BLUE, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.0f,   0.5f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.1667f,0.5f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_RIGHT, ANIMATION_TRIGGER::SHRIMP_NEUTRAL, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.3333f,0.5f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.5f,   0.5f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_RIGHT, ANIMATION_TRIGGER::SHRIMP_RED, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.6667f,0.5f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.8333f,0.5f), vec2(0.1667f, 0.25f) }}
		});

	/** UP P Shrimp **/
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_UP, ANIMATION_TRIGGER::SHRIMP_BLUE, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.0f,   0.75f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.1667f,0.75f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_UP, ANIMATION_TRIGGER::SHRIMP_NEUTRAL, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.3333f,0.75f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.5f,   0.75f), vec2(0.1667f, 0.25f) }}
		});
	generate_animation(ANIMATABLE_TYPE::PISTOL_SHRIMP_UP, ANIMATION_TRIGGER::SHRIMP_RED, {
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.6667f,0.75f), vec2(0.1667f, 0.25f) }},
		{ TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, RenderSourceRect{ vec2(0.8333f,0.75f), vec2(0.1667f, 0.25f) }}
		});

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

	/*
		ENVIRONMENT ANIMATIONS
	*/
	generate_animation(
		ANIMATABLE_TYPE::MOSS_TRIGGER,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.5, 0.125), vec2(0.25, 0.125) }},
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::MOSS_TRIGGER,
		ANIMATION_TRIGGER::IDLE_BLUE,
		{
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.25, 0.0), vec2(0.25, 0.125) }},
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.25, 0.125), vec2(0.25, 0.125) }},
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.75, 0.0), vec2(0.25, 0.125) }},
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.25, 0.125), vec2(0.25, 0.125) }},
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::MOSS_TRIGGER,
		ANIMATION_TRIGGER::IDLE_RED,
		{
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.0, 0.0), vec2(0.25, 0.125) }},
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.0, 0.125), vec2(0.25, 0.125) }},
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.5, 0.0), vec2(0.25, 0.125) }},
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.0, 0.125), vec2(0.25, 0.125) }},
		}
		);
	generate_animation(
		ANIMATABLE_TYPE::MOSS_WALL,
		ANIMATION_TRIGGER::IDLE,
		{
				{ TEXTURE_ASSET_ID::MOSS_NEUTRAL, RenderSourceRect { vec2(0.300, 0.05), vec2(0.375, 0.45) }},
		}
		);
	generate_animation(
		ANIMATABLE_TYPE::MOSS_WALL,
		ANIMATION_TRIGGER::IDLE_RED,
		{
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.3, 0.4), vec2(0.36, 0.225) }},
		}
		);
	generate_animation(
		ANIMATABLE_TYPE::MOSS_WALL,
		ANIMATION_TRIGGER::IDLE_BLUE,
		{
				{ TEXTURE_ASSET_ID::MOSS, RenderSourceRect { vec2(0.3, 0.64), vec2(0.35, 0.225) }},
		}
	);

	/*
		UI ANIMATIONS
	*/
	generate_animation(
		ANIMATABLE_TYPE::COLOUR_INDICATOR,
		ANIMATION_TRIGGER::HUD_BLUE,
		{
			{ TEXTURE_ASSET_ID::COLOUR_INDICATOR, RenderSourceRect{ vec2(0.0f, 0.0f),   vec2(0.5f, 0.333f) }},
			{ TEXTURE_ASSET_ID::COLOUR_INDICATOR, RenderSourceRect{ vec2(0.5f, 0.0f),   vec2(0.5f, 0.333f) }}
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::COLOUR_INDICATOR,
		ANIMATION_TRIGGER::HUD_NEUTRAL,
		{
			{ TEXTURE_ASSET_ID::COLOUR_INDICATOR, RenderSourceRect{ vec2(0.0f, 0.333f), vec2(0.5f, 0.333f) }},
			{ TEXTURE_ASSET_ID::COLOUR_INDICATOR, RenderSourceRect{ vec2(0.5f, 0.333f), vec2(0.5f, 0.333f) }}
		}
	);
	generate_animation(
		ANIMATABLE_TYPE::COLOUR_INDICATOR,
		ANIMATION_TRIGGER::HUD_RED,
		{
			{ TEXTURE_ASSET_ID::COLOUR_INDICATOR, RenderSourceRect{ vec2(0.0f, 0.667f), vec2(0.5f, 0.333f) }},
			{ TEXTURE_ASSET_ID::COLOUR_INDICATOR, RenderSourceRect{ vec2(0.5f, 0.667f), vec2(0.5f, 0.333f) }}
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
