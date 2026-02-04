// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move each entity that has motion (invaders, projectiles, and even towers [they have 0 for velocity])
	// based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		(void)elapsed_ms; // placeholder to silence unused warning until implemented
	}

	// check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				// CK: why the duplication, except to allow searching by entity_id
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				// registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	handle_player_movement();
}

void PhysicsSystem::handle_player_movement()
{
	// How should we assert if move_keys is uninitialized?
	ComponentContainer<MoveKeys> &move_keys_container = registry.move_keys;

	// TODO: refactor once I'm awake
	assert(move_keys_container.entities.size() == 1 && "move_keys has too many entities in it. Should only be 1.");
	if (move_keys_container.entities.size() == 0)
	{ 
		std::cout << "DEBUG: move_keys not initialized yet" << std::endl;
		return;
	}
	// Should only be one but obviously refactor later
	auto& move_keys = move_keys_container.components[0];

	// Check if 3+ keys are currently being pushed. If yes, don't move
	// @Geetika feel free to change this, I just figured that the sheep shouldn't move if more than 3 keys are being pressed
	// TODO: don't move on W + S being pressed or A + D being pressed 
	if (move_keys.movement_keys.count() >= 3)
	{
		std::cout << "DEBUG: too many keys pushed to move :(" << std::endl;
		return;
	}

	// Northwest
	if (move_keys.movement_keys[W] && move_keys.movement_keys[A])
	{
		std::cout << "DEBUG: Moving northwest!" << std::endl;
	}
	// Northeast
	else if (move_keys.movement_keys[W] && move_keys.movement_keys[D])
	{
		std::cout << "DEBUG: Moving northeast!" << std::endl;
	}
	// Southeast
	else if (move_keys.movement_keys[D] && move_keys.movement_keys[S])
	{
		std::cout << "DEBUG: Moving southeast!" << std::endl;
	}
	// Southwest
	else if (move_keys.movement_keys[A] && move_keys.movement_keys[S])
	{
		std::cout << "DEBUG: Moving southwest!" << std::endl;
	}
	// North
	else if (move_keys.movement_keys[W])
	{
		std::cout << "DEBUG: Moving north!" << std::endl;
	}
	// East
	else if (move_keys.movement_keys[D])
	{
		std::cout << "DEBUG: Moving east!" << std::endl;
	}
	// South
	else if (move_keys.movement_keys[S])
	{
		std::cout << "DEBUG: Moving south!" << std::endl;
	}
	// West
	else if (move_keys.movement_keys[A])
	{
		std::cout << "DEBUG: Moving west!" << std::endl;
	}
}