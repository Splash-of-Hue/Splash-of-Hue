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
	registry.breakables.emplace(box);
	Collidable& collidable = registry.collidables.emplace(box);
	collidable.inelastic = true;

	return box;
}

Entity createColourOrb(vec2 position, float radius, COLOUR_STATE colour_state)
{
	std::cout << "createColourOrb called, palette size: " << registry.colourPalettes.components.size() << std::endl;  // ADD THIS
	Size size = { 2.0f * radius, 2.0f * radius };
	Entity colourOrb = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::COLOUR_ORB);

	Collidable& col = registry.collidables.emplace(colourOrb);
	col.inelastic = false;
	col.overlay = true;


	ColourOrb orb;
	orb.colour = colour_state;
	registry.colourOrbs.insert(colourOrb, orb);  // THIS is what puts it in registry.colourOrbs

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

Entity createMossToggle(vec2 position, vec2 size, COLOUR_STATE color_state, std::vector<std::string> connected_wall_ids)
{
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(color_state);
	Entity& toggle_entity = createRectangle(position, size.x, size.y, RENDER_LAYER::FLOOR_INTERACTABLES, color_from_state);
	MossToggle& moss_toggle = registry.mossToggles.emplace(toggle_entity);
	ColourState& color_state_component = registry.colourStates.emplace(toggle_entity);
	color_state_component.curr_colour_state = color_state;
	moss_toggle.connected_wall_ids = connected_wall_ids;
	Collidable& col = registry.collidables.emplace(toggle_entity);
	col.inelastic = true;

	return toggle_entity;
}
