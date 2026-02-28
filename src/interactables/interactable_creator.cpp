#include "interactables/interactable_creator.hpp"
#include "ecs/registry.hpp"
#include "world_init.hpp"
#include "interactables/interactable_components.hpp"

Entity addGeneralInteractableComponents(vec2 position, Size& size, INTERACTABLE_TYPE interactable_type) {
	Entity interactable;
	Collidable& collidable = registry.collidables.emplace(interactable);
	collidable.inelastic = true;
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

	return box;
}

Entity createColourOrb(vec2 position, float radius, COLOUR_STATE colour_state) {
	Size size = { 2.0f * radius, 2.0f * radius };
	Entity colourOrb = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::COLOUR_ORB);
	registry.collectables.emplace(colourOrb);

	return colourOrb;
}

Entity createMushroom(vec2 position, float radius, COLOUR_STATE colour_state) {
	
	Size size = { 2.0f * radius, 2.0f * radius };
	Entity mushroom = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::MUSHROOM);
	registry.collectables.emplace(mushroom);	

	return mushroom;
}
