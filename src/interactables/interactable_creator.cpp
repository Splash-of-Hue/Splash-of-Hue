#include "animation/animation_constants.hpp"
#include "ecs/registry.hpp"
#include "interactables/interactable_components.hpp"
#include "interactables/interactable_creator.hpp"
#include "shapes/shape_build_init.hpp"
#include <iostream>
#include <string>
#include <vector>

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
	registry.unloadables.emplace(box, Unloadable{ position });
	//so boxes can't have light pass through them
	registry.walls.emplace(box);
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
	registry.scales.insert(colourOrb, vec2(size.width, size.height));
	registry.angles.emplace(colourOrb);

	Textured& textured = registry.textured.emplace(colourOrb);
	textured.used_texture = TEXTURE_ASSET_ID::COLOUR_ORB_SHEET;

	registry.renderRequests.insert(
		colourOrb,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR_INTERACTABLES,
		}
		);

	const Animatable animatable = {
		ANIMATABLE_TYPE::COLOUR_ORB,
		ANIMATION_BEHAVIOR::LOOP,
		ANIMATION_TRIGGER::COLOUR_ORB_IDLE,
		8
	};
	registry.animatables.insert(colourOrb, animatable);

	return colourOrb;
}

Entity createMushroom(vec2 position, float radius, COLOUR_STATE colour_state, MUSHROOM_SIZE mushroom_size)
{
	switch (mushroom_size)
	{
		case MUSHROOM_SIZE::SMALL:
			radius = static_cast<int>(MUSHROOM_WIDTH::SMALL);
			break;
		case MUSHROOM_SIZE::MEDIUM:
			radius = static_cast<int>(MUSHROOM_WIDTH::MEDIUM);
			break;
		case MUSHROOM_SIZE::LARGE:
			radius = static_cast<int>(MUSHROOM_WIDTH::LARGE);
			break;
	}
	Size size = { radius, radius };
	Entity mushroom = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::MUSHROOM);
	registry.collectables.emplace(mushroom);
	Collidable& collidable = registry.collidables.emplace(mushroom);
	collidable.inelastic = true;
	registry.circles.emplace(mushroom);
	registry.unloadables.emplace(mushroom, Unloadable{ position });
	Mushroom& mushroom_component = registry.mushrooms.emplace(mushroom);
	mushroom_component.size = mushroom_size;

	registry.scales.insert(mushroom, vec2(size.width, size.height));
	registry.colourStates.insert(mushroom, { colour_state, colour_state });
	registry.colors.insert(mushroom, vec4(0.f, 1.f, 0.f, 1.0f));
	Textured& textured = registry.textured.emplace(mushroom);
	textured.used_texture = TEXTURE_ASSET_ID::MUSHROOM;
	registry.angles.emplace(mushroom);
	registry.renderRequests.insert(
		mushroom,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR_INTERACTABLES
		}
	);
	return mushroom;
}

Entity createMossToggle(const int level_uid, vec2 position, vec2 size, std::vector<std::string> connected_wall_ids)
{
	Entity toggle_entity;
	MossToggle& moss_toggle = registry.mossToggles.insert(toggle_entity, { connected_wall_ids });
	registry.positions.insert(toggle_entity, position);
	registry.scales.insert(toggle_entity, size);
	registry.sizes.insert(toggle_entity, { size.x, size.y });
	registry.angles.emplace(toggle_entity);
	registry.levelEntities.insert(toggle_entity, { level_uid });
	Collidable& col = registry.collidables.emplace(toggle_entity);
	col.inelastic = false;
	registry.colourStates.insert(toggle_entity, { COLOUR_STATE::NEUTRAL, COLOUR_STATE::NEUTRAL });
	registry.renderRequests.insert(
		toggle_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR_INTERACTABLES
		}
	);
	const ANIMATION_TRIGGER trigger = ANIMATION_TRIGGER::IDLE;
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

Entity createPressurePlate(const int level_uid, vec2 position, vec2 size, std::vector<std::string> connected_wall_ids)
{
	Size size_component = { size.x, size.y };
	Entity pressure_plate_entity = addGeneralInteractableComponents(position, size_component, INTERACTABLE_TYPE::PRESSURE_PLATE);
	PressurePlate& pressure_plate = registry.pressurePlates.emplace(pressure_plate_entity);
	pressure_plate.connected_wall_ids = connected_wall_ids;

	registry.scales.insert(pressure_plate_entity, size);

	Collidable& col = registry.collidables.emplace(pressure_plate_entity);
	col.inelastic = false;

	InBoundingBox& in_bounding_box_component = registry.inBoundingBox.emplace(pressure_plate_entity);
	in_bounding_box_component.updatedThisStep = true;

	// todo QWNAo0jQ
	registry.rectangles.emplace(pressure_plate_entity);
	registry.colors.insert(pressure_plate_entity, vec4(0.0f, 0.0f, 0.75f, 1.0f));
	registry.renderRequests.insert(
		pressure_plate_entity,
		{
			EFFECT_ASSET_ID::DEFAULT,
			GEOMETRY_BUFFER_ID::RECTANGLE,
			RENDER_LAYER::FLOOR_INTERACTABLES
		}
	);

	return pressure_plate_entity;
}

Entity createPuzzleBubble(const int level_uid, std::string id, vec2 position, COLOUR_STATE colour_solution)
{
	Entity puzzle_bubble_entity;
	registry.puzzle_bubbles.insert(puzzle_bubble_entity, { colour_solution });
	registry.puzzle_pieces.insert(puzzle_bubble_entity, { id });
	const ColourPalette& palette = registry.colourPalettes.components[0];
	registry.colors.insert(puzzle_bubble_entity, palette.state_to_colour.at(COLOUR_STATE::NEUTRAL));
	Collidable& col = registry.collidables.emplace(puzzle_bubble_entity);
	col.inelastic = false;
	registry.positions.insert(puzzle_bubble_entity, position);
	registry.scales.insert(puzzle_bubble_entity, PUZZLE_BUBBLE_SIZE);
	registry.sizes.insert(puzzle_bubble_entity, { PUZZLE_BUBBLE_SIZE.x, PUZZLE_BUBBLE_SIZE.y });
	registry.angles.emplace(puzzle_bubble_entity);
	registry.colourStates.insert(puzzle_bubble_entity, { COLOUR_STATE::NEUTRAL, COLOUR_STATE::NEUTRAL });
	registry.renderRequests.insert(
		puzzle_bubble_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR_INTERACTABLES
		}
	);
	const Animatable animatable = {
		ANIMATABLE_TYPE::PUZZLE_BUBBLE,
		ANIMATION_BEHAVIOR::LOOP,
		ANIMATION_TRIGGER::IDLE,
		PUZZLE_BUBBLE_FRAME_RATE
	};
	registry.animatables.insert(puzzle_bubble_entity, animatable);
	registry.textured.insert(puzzle_bubble_entity, { TEXTURE_ASSET_ID::PUZZLE_BUBBLE });

	return puzzle_bubble_entity;
}

Entity createPuzzleBubbleFake(const int level_uid, vec2 position, COLOUR_STATE colour)
{
	Entity puzzle_bubble_entity;
	const ColourPalette& palette = registry.colourPalettes.components[0];
	registry.colors.insert(puzzle_bubble_entity, palette.state_to_colour.at(colour));
	registry.positions.insert(puzzle_bubble_entity, position);
	registry.scales.insert(puzzle_bubble_entity, PUZZLE_BUBBLE_SIZE);
	registry.sizes.insert(puzzle_bubble_entity, { PUZZLE_BUBBLE_SIZE.x, PUZZLE_BUBBLE_SIZE.y });
	registry.angles.emplace(puzzle_bubble_entity);
	registry.renderRequests.insert(
		puzzle_bubble_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::FLOOR_DECORATIONS
		}
	);
	registry.textured.insert(puzzle_bubble_entity, { TEXTURE_ASSET_ID::PUZZLE_BUBBLE_FAKE });

	return puzzle_bubble_entity;
}

Entity createFloorPictureControls(vec2 position, float width, float height, CONTROL_PNG_TYPE control_png_type)
{
	Size size = { width, height };
	Entity control_entity = addGeneralInteractableComponents(position, size, INTERACTABLE_TYPE::CONTROLS);

	registry.renderRequests.insert(
		control_entity,
		{
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER::WALLS_BACK
		}
	);

	ANIMATION_TRIGGER trigger;
	switch (control_png_type)
	{
	case CONTROL_PNG_TYPE::WASD:
		trigger = ANIMATION_TRIGGER::WASD;
		break;
	case CONTROL_PNG_TYPE::ARROW_NO_RED:
		trigger = ANIMATION_TRIGGER::ARROW_NO_RED;
		break;
	case CONTROL_PNG_TYPE::ARROW_RED:
		trigger = ANIMATION_TRIGGER::ARROW_RED;
		break;
	case CONTROL_PNG_TYPE::COLOUR_AFFECTS:
		trigger = ANIMATION_TRIGGER::COLOUR_AFFECTS;
		break;
	default:
		trigger = ANIMATION_TRIGGER::WASD;
		break;
	}

	const Animatable animatable = {
		ANIMATABLE_TYPE::CONTROLS_SHEET,
		ANIMATION_BEHAVIOR::LOOP,
		trigger,
		1
	};

	registry.animatables.insert(control_entity, animatable);
	registry.textured.insert(control_entity, { TEXTURE_ASSET_ID::CONTROLS_SHEET });
	registry.scales.insert(control_entity, vec2(width, height));
	registry.angles.emplace(control_entity);

	return control_entity;
}
