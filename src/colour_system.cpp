#pragma once 

#include "colour_system.hpp"
#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"

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
