#include "ecs/registry.hpp"
#include "interactables/interactable_components.hpp"
#include "interactables/interactable_creator.hpp"
#include "world_init.hpp"
#include <string>
#include <vector>
#include <iostream>

Entity addGeneralInteractableComponents(vec2 position, Size& size, INTERACTABLE_TYPE interactable_type)
{
	Entity interactable;
	registry.interactables.insert(interactable, { interactable_type });
	registry.positions.insert(interactable, position);
	registry.sizes.insert(interactable, size);
	return interactable;
}

Entity createBox(vec2 position, float width, float height)
{
	Size size = { width, height };
	Entity box = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::BOX);
	Collidable& collidable = registry.collidables.emplace(box);
	collidable.inelastic = true;
	registry.breakables.emplace(box);
	registry.boxes.emplace(box);
	registry.rectangles.emplace(box);
	registry.scales.insert(box, vec2(width, height));
	registry.colors.insert(box, vec4(0.6f, 0.4f, 0.2f, 1.0f));

	registry.renderRequests.insert(
		box,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			RENDER_LAYER::FLOOR_INTERACTABLES
		}
	);

	return box;
}

Entity createColourOrb(vec2 position, float radius, COLOUR_STATE colour_state)
{
	std::cout << "createColourOrb called, palette size: " << registry.colourPalettes.components.size() << std::endl; 
	Size size = { 2.0f * radius, 2.0f * radius };
	Entity colourOrb = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::COLOUR_ORB);
	Collidable& collidable = registry.collidables.emplace(colourOrb);
	collidable.inelastic = false;
	collidable.overlay = true;

	ColourOrb orb;
	orb.colour = colour_state;
	registry.colourOrbs.insert(colourOrb, orb); 

	const ColourPalette& palette = registry.colourPalettes.components[0];
	registry.colors.insert(colourOrb, palette.state_to_colour.at(colour_state));
	registry.circles.emplace(colourOrb);
	registry.scales.insert(colourOrb, vec2(size.width, size.height));

	registry.renderRequests.insert(
		colourOrb,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::CIRCLE,
			RENDER_LAYER::FLOOR_INTERACTABLES,
		}
	);

	return colourOrb;
}

Entity createMushroom(vec2 position, float radius, COLOUR_STATE colour_state)
{

	Size size = { 2.0f * radius, 2.0f * radius };
	Entity mushroom = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::MUSHROOM);
	registry.collectables.emplace(mushroom);
	Collidable& collidable = registry.collidables.emplace(mushroom);
	collidable.inelastic = true;

	return mushroom;
}

Entity createMossToggle(const int level_uid, vec2 position, vec2 size, COLOUR_STATE color_state, std::vector<std::string> connected_wall_ids)
{
	Entity toggle_entity;
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	MossToggle& moss_toggle = registry.mossToggles.insert(toggle_entity, { connected_wall_ids });
	registry.positions.insert(toggle_entity, position);
	registry.scales.insert(toggle_entity, size);
	registry.sizes.insert(toggle_entity, { size.x, size.y });
	registry.colors.emplace(toggle_entity, color_from_state);
	registry.angles.emplace(toggle_entity);
	Collidable& col = registry.collidables.emplace(toggle_entity);
	col.inelastic = false;
	registry.colourStates.insert(toggle_entity, { color_state, color_state });
	registry.renderRequests.insert(
		toggle_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR_INTERACTABLES
		}
	);
	const ANIMATION_TRIGGER trigger = color_state == COLOUR_STATE::NEUTRAL ? ANIMATION_TRIGGER::IDLE : color_state == COLOUR_STATE::BLUE ? ANIMATION_TRIGGER::IDLE_BLUE : ANIMATION_TRIGGER::IDLE_RED;
	const Animatable animatable = {
		ANIMATABLE_TYPE::MOSS_TRIGGER,
		ANIMATION_BEHAVIOR::LOOP,
		trigger,
		MOSS_FRAME_RATE
	};
	registry.animatables.insert(toggle_entity, animatable);
	registry.textured.insert(toggle_entity, { TEXTURE_ASSET_ID::MOSS });

	return toggle_entity;
}
