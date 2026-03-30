// internal
#include "interactables/interactable_components.hpp"
#include "map/map_constants.hpp"
#include "emission_system.hpp"
#include "emission_constants.hpp"
#include "shapes/shape_build_init.hpp"
#include "colour_system.hpp"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <numeric>

void EmissionSystem::step(float elapsed_ms)
{
	for (Entity& emission_entity : registry.lightEmissions.entities)
	{
		//mushrooms unload before emissions, meaning some emissions might not have a source
		if (!registry.colourStates.has(registry.lightEmissions.get(emission_entity).lightSourceEntity))
		{
			continue;
		}
		//Colour states is looked at instead of other components to avoid assertion below
		if (!registry.colourStates.has(emission_entity))
			initialize_emission(emission_entity);
		ColourState& emission_colour_state = registry.colourStates.get(emission_entity);
		ColourState& source_colour_state = registry.colourStates.get(registry.lightEmissions.get(emission_entity).lightSourceEntity);
		if (emission_colour_state.curr_colour_state != source_colour_state.curr_colour_state)
			change_emission_colour_state(emission_entity, source_colour_state.curr_colour_state);
		if (emission_colour_state.curr_colour_state == COLOUR_STATE::NEUTRAL)
			remove_emission(emission_entity);
		//If emission is neutral, it is deleted, so it won't have size
		if (registry.sizes.has(emission_entity) && (registry.sizes.get(emission_entity).width < registry.lightEmissions.get(emission_entity).MAX_RADIUS))
			update_emission_size(emission_entity, elapsed_ms);

		//If emission is neutral, it is deleted, so it won't have lightEmission
		if (registry.lightEmissions.has(emission_entity) && registry.velocities.has(registry.lightEmissions.get(emission_entity).lightSourceEntity))
			update_emission_position(emission_entity);
	}
}

// Adds non component-specific properties to emissions
void EmissionSystem::initialize_emission(Entity& emission_entity)
{	
	//assigning collidable hitbox to emission, so that collision with the circle are noticed
	Collidable& light_emission_collidable_component = registry.collidables.emplace(emission_entity);
	light_emission_collidable_component.overlay = true;
	light_emission_collidable_component.inelastic = false;
	//TODO: https://trello.com/c/mBipkaT3/187-1-make-light-emission-look-pretty
	RenderRequest& light_emission_render_request = registry.renderRequests.get(emission_entity);
	ColourState& colour_circle_colour_state = registry.colourStates.emplace(emission_entity);
}

// Removes emission on neutral state, otherwise changes colour state of emission & resets size for growth
void EmissionSystem::change_emission_colour_state(Entity& emission_entity, COLOUR_STATE colour_state)
{
	ColourState& emission_colour_state = registry.colourStates.get(emission_entity);
	Size& emission_size = registry.sizes.get(emission_entity);
	emission_size.width = 0.f;
	emission_size.height = 0.f;
	emission_colour_state.prev_colour_state = emission_colour_state.curr_colour_state;
	emission_colour_state.curr_colour_state = colour_state;
	
	//changes visual colour of entity
	Entity palette_entity = registry.colourPalettes.entities[0];
	const ColourPalette& palette = registry.colourPalettes.get(palette_entity);

	if (palette.state_to_colour.count(colour_state) == 0) return;

	vec4 color = palette.state_to_colour.at(colour_state);
	color.a = 0.5;

	if (!registry.colors.has(emission_entity)) {
		registry.colors.emplace(emission_entity);
	}
	registry.colors.get(emission_entity) = color;
}

void EmissionSystem::remove_emission(Entity& emission_entity)
{
	Entity& source_entity = registry.lightEmissions.get(emission_entity).lightSourceEntity;
	registry.lightSources.remove(source_entity);
	registry.remove_all_components_of(emission_entity);
	return;
}

void EmissionSystem::update_emission_size(Entity& emission_entity, float elapsed_ms)
{	
	float max_radius = registry.lightEmissions.get(emission_entity).MAX_RADIUS;
	float time_to_grow = registry.lightEmissions.get(emission_entity).TIME_TO_GROW;

	Size& light_emission_size = registry.sizes.get(emission_entity);
	if (light_emission_size.width < max_radius)
	{
		light_emission_size.width = min(max_radius, light_emission_size.width + max_radius * elapsed_ms / time_to_grow);
		light_emission_size.height = min(max_radius, light_emission_size.height + max_radius * elapsed_ms / time_to_grow);
	}
}

void EmissionSystem::update_emission_position(Entity& emission_entity)
{
	if (registry.levelEntities.has(emission_entity))
	{
		LevelEntity& emission_level_entity = registry.levelEntities.get(emission_entity);
		emission_level_entity.level_uid = registry.levelEntities.get(registry.lightEmissions.get(emission_entity).lightSourceEntity).level_uid;
	}
	vec2& source_position = registry.positions.get(registry.lightEmissions.get(emission_entity).lightSourceEntity);
	vec2& emission_position = registry.positions.get(emission_entity);
	
	//rotating creatures (anglerfish) will have an offset to the emission
	if (registry.angles.has(registry.lightEmissions.get(emission_entity).lightSourceEntity))
	{
		LightEmission& emission_component = registry.lightEmissions.get(emission_entity);
		float angle = registry.angles.get(registry.lightEmissions.get(emission_entity).lightSourceEntity).degrees;
		angle = angle * 3.14 / 180;
		//change position based off offset and creature rotation
		emission_position.x = source_position.x + emission_component.offset.x * cos(angle) - emission_component.offset.y * sin(angle);
		emission_position.y = source_position.y + emission_component.offset.x * sin(angle) + emission_component.offset.y * cos(angle);
		//covers entity flipping while its moving 
		if (registry.velocities.has(registry.lightEmissions.get(emission_entity).lightSourceEntity))
		{
			vec2 source_velocity = registry.velocities.get(registry.lightEmissions.get(emission_entity).lightSourceEntity);
			if (source_velocity.x < 0)
				emission_position.x = source_position.x - emission_component.offset.x * cos(angle) + emission_component.offset.y * sin(angle);
		}
		return;
	}

	emission_position.x = source_position.x;
	emission_position.y = source_position.y;
}
