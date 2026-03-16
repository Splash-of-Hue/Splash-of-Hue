// internal
#include "interactables/interactable_components.hpp"
#include "map/map_constants.hpp"
#include "emission_system.hpp"
#include "emission_constants.hpp"
#include "shapes/shape_build_init.hpp"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <numeric>

float getRadiusSize(Entity& source_entity)
{
	if (registry.players.has(source_entity))
		return typeOfEntityToEmissionSize[0];

	if (registry.mushrooms.has(source_entity))
		return typeOfEntityToEmissionSize[1 + (int)registry.mushrooms.get(source_entity).size];

	return 0;
}

vec2 getOffset(Entity& source_entity)
{
	//todo: once we start rotating the anglerfish, we will need to change this
	return { 0,0 };
}

void EmissionSystem::create_new_emissions()
{
	for (Entity& source_entity : registry.markedForEmissionCreation.entities)
	{
		assert(registry.positions.has(source_entity), "33");
		vec2& position = registry.positions.get(source_entity);
		vec4 light_emission_colour = DEFAULT_COLOUR_VEC4;
		light_emission_colour.a = 0.2f;

		Entity& light_emission_entity = createCircle(position, 1.f, RENDER_LAYER::PLAYER, light_emission_colour);
		Collidable& light_emission_collidable_component = registry.collidables.emplace(light_emission_entity);
		light_emission_collidable_component.overlay = true;
		light_emission_collidable_component.inelastic = false;

		LightEmission& light_emission_component = registry.lightEmissions.emplace(light_emission_entity);
		light_emission_component.MAX_RADIUS = getRadiusSize(source_entity);
		light_emission_component.offset = getOffset(source_entity);
		light_emission_component.TIME_TO_GROW = 1000.f;
		light_emission_component.lightSourceEntity = source_entity;
		assert(registry.renderRequests.has(light_emission_entity), "33");
		RenderRequest& light_emission_render_request = registry.renderRequests.get(light_emission_entity);
		light_emission_render_request.used_geometry = GEOMETRY_BUFFER_ID::GRADIENT_CIRCLE;

		ColourState& colour_circle_colour_state = registry.colourStates.emplace(light_emission_entity);
	
		LightSource& light_source_component = registry.lightSources.emplace(source_entity);
		light_source_component.lightEmissionEntity = light_emission_entity;

		registry.markedForEmissionCreation.remove(source_entity);
	}
}

void EmissionSystem::change_emission_colour()
{
	for (Entity& source_entity : registry.markedForEmissionColourChanges.entities)
	{
		if (!registry.lightSources.has(source_entity))
			return;
		assert(registry.lightSources.has(source_entity), "66");
		LightSource& light_source_component = registry.lightSources.get(source_entity);
		Entity& emission_entity = light_source_component.lightEmissionEntity;

		if (!registry.colourStates.has(emission_entity) || !registry.sizes.has(emission_entity) || !registry.colourStates.has(source_entity))
			return;
		assert(registry.colourStates.has(source_entity), "72");
		assert(registry.colourStates.has(emission_entity), "73");
		ColourState& source_colour_state = registry.colourStates.get(source_entity);
		ColourState& emission_colour_state = registry.colourStates.get(emission_entity);

		if (source_colour_state.curr_colour_state == emission_colour_state.curr_colour_state)
		{
			registry.markedForEmissionColourChanges.remove(source_entity);
			return;
		}
		assert(registry.sizes.has(emission_entity), "72");
		Size& emission_size = registry.sizes.get(emission_entity);
		emission_size.width = 0.f;
		emission_size.height = 0.f;

		emission_colour_state.prev_colour_state = emission_colour_state.curr_colour_state;
		emission_colour_state.curr_colour_state = source_colour_state.curr_colour_state;

		//we don't remove the markedForEmissionColourChanges so that colour_system.cpp can change the colour of the emission
	}
}

void EmissionSystem::update_emission_position_size(float elapsed_ms)
{
	for (Entity& light_emission_entity : registry.lightEmissions.entities)
	{
		if (!registry.positions.has(light_emission_entity) || !registry.sizes.has(light_emission_entity))
			return;
		//circle updates to location of whatever its attached to
		assert(registry.positions.has(light_emission_entity), "100");
		vec2& light_emission_position = registry.positions.get(light_emission_entity);
		assert(registry.lightEmissions.has(light_emission_entity), "102");
		LightEmission& light_emission_component = registry.lightEmissions.get(light_emission_entity);
		Entity& source_entity = light_emission_component.lightSourceEntity;
		assert(registry.positions.has(source_entity), "105");
		vec2& source_position = registry.positions.get(source_entity);

		light_emission_position = source_position + light_emission_component.offset;
		assert(registry.sizes.has(light_emission_entity), "109");
		Size& light_emission_size = registry.sizes.get(light_emission_entity);

		if (light_emission_size.width < light_emission_component.MAX_RADIUS)
		{
			light_emission_size.width = min(light_emission_component.MAX_RADIUS,
				light_emission_size.width + light_emission_component.MAX_RADIUS * elapsed_ms / light_emission_component.TIME_TO_GROW);
			light_emission_size.height = min(light_emission_component.MAX_RADIUS,
				light_emission_size.height + light_emission_component.MAX_RADIUS * elapsed_ms / light_emission_component.TIME_TO_GROW);
		}
	}
}

void EmissionSystem::remove_emissions()
{
	for (Entity& source_entity : registry.markedForEmissionRemoval.entities)
	{
		if (!registry.lightSources.has(source_entity))
		{
			return;
		}

		LightSource& light_source_component = registry.lightSources.get(source_entity);
		Entity& emission_entity = light_source_component.lightEmissionEntity;

		registry.remove_all_components_of(emission_entity);
		registry.lightSources.remove(source_entity);
		registry.markedForEmissionRemoval.remove(source_entity);
	}
}

void EmissionSystem::step(float elapsed_ms)
{
	//first, we check if there are any entities that are going to emit, but havent yet, give them an emission, but this emission does not have a colour yet
	create_new_emissions();
	//then, we look at any sources that have a different colour state than their emission, and change their emission accordingly
	change_emission_colour();
	//then, we update their position and size if they need to be changed.
	update_emission_position_size(elapsed_ms);
	//finally, we remove any emissions marked as needing to remove emission
	remove_emissions();
}
