#pragma once 

#include "colour_system.hpp"
#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
ColourSystem::ColourSystem() {}

// SPLASH OF HUE Colour System
auto& players = registry.players;
auto& colours = registry.colours;
auto& pistolShrimps = registry.pistolShrimps;
auto& deadlys = registry.deadlys;
// function is called when collision event happens, Event handler system can pass over to other systems
void ColourSystem::on_collision(Entity collider, Entity collided) 
{
	// collision is b/w player and creature that has the same colour HANDLED IN OTHER SYSTEM!
	// collision is b/w entity and deadly object 
	if (deadlys.has(collided)) {
		if (registry.deadlys.has(collided)) {
			// entity dies? would other entities die if hit by deadly ob
			return;
		}
	}
	
	// collision is b/w coloured player and coloured object (coloured different from player)
	// NOTE: if fish passes by mushroom tho it can also have colour effects 
	if (players.has(collider) && colours.has(collider) && colours.has(collided)) {
		Colour& player_colour_comp = colours.get(collider);
		Colour& other_colour_comp = colours.get(collided);
		
		player_colour_comp.colour_state = other_colour_comp.colour_state;
	}
}

COLOUR_STATE ColourSystem::getEntityColour(Entity entity) {
    if (registry.colours.has(entity)) {
        return registry.colours.get(entity).colour_state;
    }
	// TODO: should this be an error instead? 
    return COLOUR_STATE::NEUTRAL;
}

void ColourSystem::applyColour(Entity entity, COLOUR_STATE colour) {
	if (!registry.colours.has(entity)) {
		registry.colours.emplace(entity);
	}
	else {
		registry.colours.get(entity).colour_state = colour;
	}
}

vec3 ColourSystem::getPaletteColour(COLOUR_STATE state) {
    // Find the palette entity (you might want to make this a singleton or pass it in)
    for (Entity entity : registry.colourPalettes.entities) {
        const auto& palette = registry.colourPalettes.get(entity);
        auto it = palette.state_to_colour.find(state);
        return (it != palette.state_to_colour.end()) ? it->second : vec3(1.0f, 1.0f, 1.0f);
    }
    return vec4(1.0f, 1.0f, 1.0f, 0.5f); // fallback
}
