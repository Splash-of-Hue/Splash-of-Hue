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
		if (registry.colourStates.has(player)) {
			initial_colour = registry.colourStates.get(player).curr_colour_state;
		}
	}

	colour_menu_entity = createRectangle(top_left, bottom_right, RENDER_LAYER::UI, vec4(0.2f, 0.2f, 0.2f, 1.0f));

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

	if (!registry.colourStates.has(player)) {
		registry.colourStates.emplace(player);
	}
	registry.colourStates.get(player).curr_colour_state = chosen;

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

	if (!colour_menu_open) return;
	assert(!registry.colourMenuStates.entities.empty() && "Colour menu states should not be empty");

	if (registry.colourWheels.has(colour_menu_entity)) {
		ColourWheel& wheel = registry.colourWheels.get(colour_menu_entity);
		int check = control.value_1;

		int curr_idx = static_cast<int>(wheel.curr_colour);
		int num_colors = 3;
		curr_idx = (curr_idx + check + num_colors) % num_colors;
		wheel.curr_colour = static_cast<COLOUR_STATE>(curr_idx);

		setEntityVisualColour(colour_menu_entity, wheel.curr_colour);
	}
}

COLOUR_STATE ColourSystem::getEntityColour(Entity entity) {
    if (registry.colourStates.has(entity)) {
        return registry.colourStates.get(entity).curr_colour_state;
    }
    return COLOUR_STATE::NEUTRAL;
}

void ColourSystem::applyColour(Entity entity, COLOUR_STATE colour) {
	if (!registry.colourStates.has(entity)) {
		registry.colourStates.emplace(entity);
	}
	registry.colourStates.get(entity).curr_colour_state = colour;
}

vec4 ColourSystem::getPaletteColour(COLOUR_STATE state) {
	assert(!registry.colourPalettes.entities.empty() && "Colour palette should be initialized");
	Entity palette_entity = registry.colourPalettes.entities[0];
	const ColourPalette& palette = registry.colourPalettes.get(palette_entity);

	return palette.state_to_colour.count(state) > 0
		? palette.state_to_colour.at(state)
		: DEFAULT_COLOUR_VEC4; 
}

void ColourSystem::applyColorToPlayer(COLOUR_STATE colour)
{
	if (registry.players.entities.empty()) {
		return;
	}

	Entity& player = registry.players.entities[0];
	Player& playerComp = registry.players.get(player);

	// Check if the colour is unlocked (by collecting the orb) before applying
	if (colour == COLOUR_STATE::BLUE && !playerComp.blue_unlocked)
	{
		std::cout << "BLUE not unlocked yet" << std::endl;
		return;
	}
	if (colour == COLOUR_STATE::RED && !playerComp.red_unlocked)
	{
		std::cout << "RED not unlocked yet" << std::endl;
		return;
	}

	if (!registry.colourStates.has(player)) {
		registry.colourStates.emplace(player);
	}
	ColourState& player_colour_state = registry.colourStates.get(player);
	player_colour_state.prev_colour_state = player_colour_state.curr_colour_state;
	player_colour_state.curr_colour_state = colour;
	
	//TODO: https://trello.com/c/XWeiA55L/149-1-colour-menu-ui-always-visible
	setEntityVisualColour(player, colour);
	changeLightEmissions(player, static_cast<int>(player_colour_state.curr_colour_state), static_cast<int>(player_colour_state.prev_colour_state));
	std::cout << "Player colour set to: " << static_cast<int>(colour) << std::endl;
}

void ColourSystem::step(float elapsed_ms)
{
	(void)elapsed_ms; // unused for now

	std::vector<Entity> to_remove;
	for (Entity& entity : registry.controls.entities)
	{
		Control control = registry.controls.get(entity);

		if (control.phase != CONTROL_PHASE::START) {
			continue;
		}

		if (control.type == CONTROL_TYPE::SET_COLOUR) {
			COLOUR_CONTROL_VALUE colour_value = static_cast<COLOUR_CONTROL_VALUE>((int)control.value_1);
			switch (colour_value)
			{
			case COLOUR_CONTROL_VALUE::BLUE:
				applyColorToPlayer(COLOUR_STATE::BLUE);
				break;
			case COLOUR_CONTROL_VALUE::RED:
				applyColorToPlayer(COLOUR_STATE::RED);
				break;
			case COLOUR_CONTROL_VALUE::NEUTRAL:
				applyColorToPlayer(COLOUR_STATE::NEUTRAL);
				break;
			default:
				break;
			}
		}
	}
	updateEmissions(elapsed_ms);
}

void ColourSystem::changeLightEmissions(Entity& player, int player_colour_state, int player_colour_state_prev)
{
	//if neutral, delete colour circle
	if (player_colour_state == static_cast<int>(COLOUR_STATE::NEUTRAL)) {
		for (Entity& entity_in_player_colour_radius : registry.inPlayerRadius.entities)
		{
			registry.inPlayerRadius.remove(entity_in_player_colour_radius);
		}
		registry.lightSources.remove(player);
		registry.remove_all_components_of(players_light_emission);
		return;
	}

	//if same colour, don't reanimate
	if (player_colour_state == player_colour_state_prev)
	{
		return;
	}
	//if player already has light emission, but the light is a different colour,
	//make size 0, so that you can see the regrowing of the circle
	if (registry.lightSources.has(player))
	{
		LightSource& light_source_component = registry.lightSources.get(player);
		players_light_emission = light_source_component.lightEmissionEntity;
		Size& colour_circle_size = registry.sizes.get(players_light_emission);
		colour_circle_size.width = 0;
		colour_circle_size.height = 0;
	}

	//create lightsource if player does not have one
	else {
		vec2& player_position = registry.positions.get(player);
		vec4 light_emission_colour = DEFAULT_COLOUR_VEC4;
		light_emission_colour.a = 0.2f;

		players_light_emission = createCircle(player_position, 1.f, RENDER_LAYER::PLAYER, light_emission_colour);
		Collidable& light_emission_collidable_component = registry.collidables.emplace(players_light_emission);
		light_emission_collidable_component.overlay = true;
		light_emission_collidable_component.inelastic = false;

		LightEmission& light_emission_component = registry.lightEmissions.emplace(players_light_emission);
		light_emission_component.MAX_RADIUS = 150.f;
		light_emission_component.TIME_TO_GROW = 1000.f;
		light_emission_component.offset = { 0.f, 0.f };    //change these numbers to test light emission offset of the player.

		RenderRequest& light_emission_render_request = registry.renderRequests.get(players_light_emission);
		light_emission_render_request.used_geometry = GEOMETRY_BUFFER_ID::GRADIENT_CIRCLE;

		ColourState& colour_circle_colour_state = registry.colourStates.emplace(players_light_emission);
		light_emission_component.lightSourceEntity = player;

		LightSource& light_source_component = registry.lightSources.emplace(player);
		light_source_component.lightEmissionEntity = players_light_emission;
	}

	ColourState& light_emission_colour_state = registry.colourStates.get(players_light_emission);
	light_emission_colour_state.prev_colour_state = light_emission_colour_state.curr_colour_state;
	light_emission_colour_state.curr_colour_state = COLOUR_STATE(player_colour_state);
	setEntityVisualColour(players_light_emission, COLOUR_STATE(player_colour_state));
}

void ColourSystem::updateEmissions(float elapsed_ms)
{
	for (Entity& light_emission_entity : registry.lightEmissions.entities)
	{
		//circle updates to location of whatever its attached to
		vec2& light_emission_position = registry.positions.get(light_emission_entity);
		
		LightEmission& light_emission_component = registry.lightEmissions.get(light_emission_entity);
		Entity& source_entity = light_emission_component.lightSourceEntity;
		vec2& source_position = registry.positions.get(source_entity);
		
		light_emission_position = source_position + light_emission_component.offset;

		Size& light_emission_size = registry.sizes.get(light_emission_entity);
		if (light_emission_size.width < light_emission_component.MAX_RADIUS){
			light_emission_size.width = min(light_emission_component.MAX_RADIUS,
											light_emission_size.width + light_emission_component.MAX_RADIUS * elapsed_ms / light_emission_component.TIME_TO_GROW);
			light_emission_size.height = min(light_emission_component.MAX_RADIUS,
				light_emission_size.height + light_emission_component.MAX_RADIUS * elapsed_ms / light_emission_component.TIME_TO_GROW);
		}
	}
}
