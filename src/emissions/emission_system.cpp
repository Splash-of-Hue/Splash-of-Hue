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
		//An emission that is created through the interactable or colour system will not have any components attached besides lightEmissions
		//This means if the entity does not have a colour state, we need to initialize the emission first
		if (!registry.colourStates.has(emission_entity))
			initialize_emission(emission_entity);
		//we can use any component to check if the emission has been initialized, but the colour state is used specifically so that we can assure the 
		//emission has a colour state for the next step, which is to compare the colour state of the emission and its source, so that we can change the 
		//emissions colour state accordingly.
		ColourState& emission_colour_state = registry.colourStates.get(emission_entity);
		ColourState& source_colour_state = registry.colourStates.get(registry.lightEmissions.get(emission_entity).lightSourceEntity);
		if (emission_colour_state.curr_colour_state != source_colour_state.curr_colour_state)
			change_emission_colour_state(emission_entity, source_colour_state.curr_colour_state);
		//Finally, we want to look at any emission that isnt fully grown yet. If the emission hasn't been deleted due to it being neutral, then it should have a size, 
		//So we will check to see if it has a size, and if that size is less than its max_radius (which is defined in interactables or colour system.
		if (registry.sizes.has(emission_entity) && (registry.sizes.get(emission_entity).width < registry.lightEmissions.get(emission_entity).MAX_RADIUS))
			update_emission_size(emission_entity, elapsed_ms);
		//Some emissions will be on a moving entity. To deal with this, we will have the position of radius update to the moving entities position. 
		//If we only look at sources that have a velocity, we won't be updating the position of every single radius on every frame. 
		//The position of the emission is updated instead of velocity, as travelling through a room with an emission active creates unintended results when using velocity.
		if (registry.lightEmissions.has(emission_entity) && registry.velocities.has(registry.lightEmissions.get(emission_entity).lightSourceEntity))
			update_emission_position(emission_entity);
	}
}

//creates a new emission. this function will not change the colour, but will add every component to the emission entity, as well as link the source and the emission.
void EmissionSystem::initialize_emission(Entity& emission_entity)
{	
	//assigning collidable hitbox to emission, so that collision with the circle are noticed
	Collidable& light_emission_collidable_component = registry.collidables.emplace(emission_entity);
	light_emission_collidable_component.overlay = true;
	light_emission_collidable_component.inelastic = false;
	//TODO: https://trello.com/c/mBipkaT3/187-1-make-light-emission-look-pretty
	RenderRequest& light_emission_render_request = registry.renderRequests.get(emission_entity);
	//Starts as neutral, but will be changed as the for loop will recognize that the colour states of the emission and the source are not equal
	ColourState& colour_circle_colour_state = registry.colourStates.emplace(emission_entity);
}
//this function only changes the state of the emission. Changing the state of emission means deleting the emission if it is neutral, changing the colour state if its red or blue
//and resetting the size to 0 if its red or blue, allowing the emission to grow.
void EmissionSystem::change_emission_colour_state(Entity& emission_entity, COLOUR_STATE colour_state)
{
	//If the entity or player is neutral. We will delete the emission. This is done by removing all components of the emission entity, but also removing the 
	//light source component from the emitting entity
	if (colour_state == COLOUR_STATE::NEUTRAL)
	{
		Entity& source_entity = registry.lightEmissions.get(emission_entity).lightSourceEntity;
		registry.lightSources.remove(source_entity);
		registry.remove_all_components_of(emission_entity);
		return;
	}
	//Otherwise, we will change the colour state, as well as changing the visual colour by calling colour_system.cpp
	ColourState& emission_colour_state = registry.colourStates.get(emission_entity);
	Size& emission_size = registry.sizes.get(emission_entity);
	emission_size.width = 0.f;
	emission_size.height = 0.f;
	emission_colour_state.prev_colour_state = emission_colour_state.curr_colour_state;
	emission_colour_state.curr_colour_state = colour_state;

	setEntityVisualColour(emission_entity, colour_state);
}
//seperating the update size and update position increases optimization somewhat. this way, these functions are only called if size needs to be updated, 
//or if the light source is a moving entity
void EmissionSystem::update_emission_size(Entity& emission_entity, float elapsed_ms)
{	
	float max_radius = registry.lightEmissions.get(emission_entity).MAX_RADIUS;
	float time_to_grow = registry.lightEmissions.get(emission_entity).TIME_TO_GROW;
	//have the light grow if it is not its maximum size.
	Size& light_emission_size = registry.sizes.get(emission_entity);
	if (light_emission_size.width < max_radius)
	{
		light_emission_size.width = min(max_radius, light_emission_size.width + max_radius * elapsed_ms / time_to_grow);
		light_emission_size.height = min(max_radius, light_emission_size.height + max_radius * elapsed_ms / time_to_grow);
	}
}

void EmissionSystem::update_emission_position(Entity& emission_entity)
{
	vec2& source_position = registry.positions.get(registry.lightEmissions.get(emission_entity).lightSourceEntity);
	vec2& emission_position = registry.positions.get(emission_entity);
	emission_position.x = source_position.x;
	emission_position.y = source_position.y;
}
