// internal
#include "interactables/interactable_components.hpp"
#include "map/map_constants.hpp"
#include "emission_system.hpp"
#include "shapes/shape_build_init.hpp"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <numeric>

float getRadiusSize(Entity& source_entity)
{
	if (registry.players.has(source_entity))
		return 150;
	if (registry.mushrooms.has(source_entity))
	{
		Mushroom& mushroom_component = registry.mushrooms.get(source_entity);
		switch (mushroom_component.size)
		{
			case MUSHROOM_SIZE::SMALL:
				return 100;
				break;
			case MUSHROOM_SIZE::MEDIUM:
				return 300;
				break;
			case MUSHROOM_SIZE::LARGE:
				return 500;
				break;
		}
	}

	return 150;
}

vec2 getOffset(Entity& source_entity)
{
	//todo: once we start rotating the anglerfish, we will need to change this
	return { 0,0 };
}

void EmissionSystem::createNewEmissions()
{
	

	for (Entity& source_entity : registry.needsToEmits.entities)
	{
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

		RenderRequest& light_emission_render_request = registry.renderRequests.get(light_emission_entity);
		light_emission_render_request.used_geometry = GEOMETRY_BUFFER_ID::GRADIENT_CIRCLE;

		ColourState& colour_circle_colour_state = registry.colourStates.emplace(light_emission_entity);
	
		LightSource& light_source_component = registry.lightSources.emplace(source_entity);
		light_source_component.lightEmissionEntity = light_emission_entity;

		registry.needsToEmits.remove(source_entity);
	}
}

void EmissionSystem::changeDifferentEmissions()
{
	for (Entity& source_entity : registry.needsToChangeColourStates.entities)
	{
		if (!registry.lightSources.has(source_entity))
		{
			return;
		}
		LightSource& light_source_component = registry.lightSources.get(source_entity);
		Entity& emission_entity = light_source_component.lightEmissionEntity;

		ColourState& source_colour_state = registry.colourStates.get(source_entity);
		ColourState& emission_colour_state = registry.colourStates.get(emission_entity);

		if (source_colour_state.curr_colour_state == emission_colour_state.curr_colour_state)
		{
			registry.needsToChangeColourStates.remove(source_entity);
			return;
		}
		Size& emission_size = registry.sizes.get(emission_entity);
		emission_size.width = 0.f;
		emission_size.height = 0.f;

		emission_colour_state.prev_colour_state = emission_colour_state.curr_colour_state;
		emission_colour_state.curr_colour_state = source_colour_state.curr_colour_state;

	}
}

void EmissionSystem::updateCurrentEmissions(float elapsed_ms)
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
		if (light_emission_size.width < light_emission_component.MAX_RADIUS)
		{
			light_emission_size.width = min(light_emission_component.MAX_RADIUS,
				light_emission_size.width + light_emission_component.MAX_RADIUS * elapsed_ms / light_emission_component.TIME_TO_GROW);
			light_emission_size.height = min(light_emission_component.MAX_RADIUS,
				light_emission_size.height + light_emission_component.MAX_RADIUS * elapsed_ms / light_emission_component.TIME_TO_GROW);
		}
	}
}

void EmissionSystem::removeEmissions()
{
	for (Entity& source_entity : registry.needsToRemoveEmissions.entities)
	{
		if (!registry.lightSources.has(source_entity))
		{
			return;
		}
		LightSource& light_source_component = registry.lightSources.get(source_entity);
		Entity& emission_entity = light_source_component.lightEmissionEntity;

		registry.remove_all_components_of(emission_entity);
		registry.lightSources.remove(source_entity);
		registry.needsToRemoveEmissions.remove(source_entity);
	}
}

void EmissionSystem::step(float elapsed_ms)
{
	//first, we check if there are any entities that are going to emit, but havent yet, give them an emission, but this emission does not have a colour yet
	createNewEmissions();
	//then, we look at any sources that have a different colour state than their emission, and change their emission accordingly
	changeDifferentEmissions();
	//then, we update their position and size if they need to be changed.
	updateCurrentEmissions(elapsed_ms);
	//finally, we remove any emissions marked as needing to remove emission
	removeEmissions();
}

