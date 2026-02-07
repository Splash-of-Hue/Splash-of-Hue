#pragma once 

#include "colour_system.hpp"
#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "world_init.hpp"
#include <iostream>
#include <vector>

ColourSystem::ColourSystem()
{
}

void ColourSystem::initialize_palette()
{
	// create palette if missing
	if (registry.colourPalettes.entities.empty()) {
		colour_palette_entity = Entity();
		registry.colourPalettes.emplace(colour_palette_entity);
	}

	// create menu state if missing
	if (registry.colourMenuStates.entities.empty()) {
		Entity ctrl = Entity();
		registry.colourMenuStates.emplace(ctrl);
		auto& state = registry.colourMenuStates.get(ctrl);
		// defaults already set by component, but be explicit
		state.is_curr_open = false;
		state.just_closed = false;
	}
}
void ColourSystem::setEntityVisualColour(Entity entity, COLOUR_STATE colour_state) {
	assert(!registry.colourPalettes.entities.empty() && "Colour palette should be initialized");

	Entity palette_entity = registry.colourPalettes.entities[0];
	const ColourPalette& palette = registry.colourPalettes.get(palette_entity);

	if (palette.state_to_colour.count(colour_state) == 0) return;

	vec4 color = palette.state_to_colour.at(colour_state);

	if (!registry.colors.has(entity)) {
		registry.colors.emplace(entity);
	}
	registry.colors.get(entity) = color;
}
void ColourSystem::openColourMenu()
{
	vec2 top_left = vec2(WINDOW_WIDTH_PX * 0.05f, WINDOW_HEIGHT_PX * 0.70f);
	vec2 bottom_right = vec2(WINDOW_WIDTH_PX * 0.20f, WINDOW_HEIGHT_PX * 0.85f);

	COLOUR_STATE initial_colour = COLOUR_STATE::NEUTRAL;
	if (!registry.players.entities.empty()) {
		Entity player = registry.players.entities[0];
		if (registry.colours.has(player)) {
			initial_colour = registry.colours.get(player).curr_colour_state;
		}
	}

	colour_menu_entity = createRectangle(top_left, bottom_right, vec4(0.2f, 0.2f, 0.2f, 1.0f));

	ColourWheel wheel;
	wheel.curr_colour = initial_colour;
	wheel.prev_colour = initial_colour;
	registry.colourWheels.insert(colour_menu_entity, wheel);

	setEntityVisualColour(colour_menu_entity, initial_colour);

	colour_menu_open = true;
}

// close UI and apply chosen colour 
void ColourSystem::closeColourMenu()
{
	COLOUR_STATE chosen = COLOUR_STATE::NEUTRAL;
	if (registry.colourWheels.has(colour_menu_entity)) {
		chosen = registry.colourWheels.get(colour_menu_entity).curr_colour;
	}

	registry.remove_all_components_of(colour_menu_entity);

	// apply colour to player
	assert(!registry.players.entities.empty() && "Player should exist when closing colour menu");
	Entity& player = registry.players.entities[0];

	if (!registry.colours.has(player)) {
		registry.colours.emplace(player);
	}
	registry.colours.get(player).curr_colour_state = chosen;

	setEntityVisualColour(player, chosen);
	
	colour_menu_open = false;
}

void ColourSystem::pollMenuState()
{
	assert(!registry.colourMenuStates.entities.empty() && "Colour menu states should not be empty");
	Entity& ctrl = registry.colourMenuStates.entities[0];
	ColourMenuState& state = registry.colourMenuStates.get(ctrl);

	bool prev = state.is_curr_open;
	state.is_curr_open = !prev;

	state.just_closed = !state.is_curr_open ? true : false;

	std::cout << "Colour menu requested " << (state.is_curr_open ? "open" : "closed") << std::endl;
	if (registry.colourMenuStates.entities.empty()) return;

	/*Entity& controller = registry.colourMenuStates.entities[0];
	ColourMenuState &state = registry.colourMenuStates.get(controller);*/

	if (state.is_curr_open && !colour_menu_open) {
		openColourMenu();
		state.just_closed = false;
	}
	if (state.just_closed && colour_menu_open) {
		closeColourMenu();
		state.just_closed = false;
		state.is_curr_open = false;
	}
}

void ColourSystem::pollColourMenuInput(Control control)
{
	assert(!registry.colourMenuStates.entities.empty() && "Colour menu states should not be empty");
	Entity& ctrl = registry.colourMenuStates.entities[0];

	if (!registry.colourMenuInputs.has(ctrl)) {
		registry.colourMenuInputs.emplace(ctrl);
	}

	/*ColourMenuInput& nav_input = registry.colourMenuInputs.get(ctrl);

	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_DOWN) {
		nav_input.direction = 1;
	}
	else if (key == GLFW_KEY_LEFT || key == GLFW_KEY_UP) {
		nav_input.direction = -1;
	}*/

	if (!colour_menu_open) return;
	assert(!registry.colourMenuStates.entities.empty() && "Colour menu states should not be empty");

	/*Entity& controller = registry.colourMenuStates.entities[0];

	if (!registry.colourMenuInputs.has(controller)) return;

	ColourMenuInput& nav_input = registry.colourMenuInputs.get(controller);

	if (nav_input.direction == 0) return;*/

	if (registry.colourWheels.has(colour_menu_entity)) {
		ColourWheel& wheel = registry.colourWheels.get(colour_menu_entity);
		int check = control.value_1;

		int curr_idx = static_cast<int>(wheel.curr_colour);
		int num_colors = 3;
		curr_idx = (curr_idx + check + num_colors) % num_colors;
		wheel.curr_colour = static_cast<COLOUR_STATE>(curr_idx);

		setEntityVisualColour(colour_menu_entity, wheel.curr_colour);
	}

	//// Clear the navigation input after processing
	//nav_input.direction = 0;
}

COLOUR_STATE ColourSystem::getEntityColour(Entity entity) {
    if (registry.colours.has(entity)) {
        return registry.colours.get(entity).curr_colour_state;
    }
    return COLOUR_STATE::NEUTRAL;
}

void ColourSystem::applyColour(Entity entity, COLOUR_STATE colour) {
	if (!registry.colours.has(entity)) {
		registry.colours.emplace(entity);
	}
	registry.colours.get(entity).curr_colour_state = colour;
}

vec4 ColourSystem::getPaletteColour(COLOUR_STATE state) {
	assert(!registry.colourPalettes.entities.empty() && "Colour palette should be initialized");
	Entity palette_entity = registry.colourPalettes.entities[0];
	const ColourPalette& palette = registry.colourPalettes.get(palette_entity);

	return palette.state_to_colour.count(state) > 0
		? palette.state_to_colour.at(state)
		: DEFAULT_COLOUR; 
}

void ColourSystem::step(float elapsed_ms)
{
	(void)elapsed_ms; // unused for now

	// Process controls produced by the conversion system.
	// Only handle toggle on key RELEASE (END) so a single press toggles once.
	// Handle selection change on key PRESS (START) or SUSTAIN (repeat).
	// Remove control entries after processing so they aren't re-processed every frame.
	std::vector<Entity> to_remove;
	for (auto& entity : registry.controls.entities)
	{
		Control control = registry.controls.get(entity);

		if (control.type == CONTROL_TYPE::TOGGLE_COLOUR_MENU && control.phase == CONTROL_PHASE::END)
		{
			pollMenuState();
		}
		if (control.type == CONTROL_TYPE::CHANGE_COLOUR_MENU_SELECTION &&
			(control.phase == CONTROL_PHASE::START || control.phase == CONTROL_PHASE::SUSTAIN))
		{
			pollColourMenuInput(control);
		}

		to_remove.push_back(entity);
	}

	for (Entity e : to_remove) {
		registry.controls.remove(e);
	}
}

/*

// handling E press to toggle colour menu (open & closed)
	if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
		assert(!registry.colourMenuStates.entities.empty() && "Colour menu states should not be empty");
		Entity& ctrl = registry.colourMenuStates.entities[0];
		ColourMenuState &state = registry.colourMenuStates.get(ctrl);

		bool prev = state.is_curr_open;
		state.is_curr_open = !prev;

		state.just_closed = !state.is_curr_open ? true : false;

		std::cout << "Colour menu requested " << (state.is_curr_open ? "open" : "closed") << std::endl;
	}

	// handling arrow keys to cycle between colour meny colours
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT ||
			key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {

			assert(!registry.colourMenuStates.entities.empty() && "Colour menu states should not be empty");
			Entity& ctrl = registry.colourMenuStates.entities[0];

			if (!registry.colourMenuInputs.has(ctrl)) {
				registry.colourMenuInputs.emplace(ctrl);
			}

			ColourMenuInput& nav_input = registry.colourMenuInputs.get(ctrl);

			if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_DOWN) {
				nav_input.direction = 1;
			}
			else if (key == GLFW_KEY_LEFT || key == GLFW_KEY_UP) {
				nav_input.direction = -1;
			}
		}
	}
*/
