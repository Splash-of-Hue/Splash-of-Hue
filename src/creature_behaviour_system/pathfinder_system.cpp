// stlib
#include <iostream>
#include <sstream>

#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"
#include "creature_behaviour_system/creature_components.hpp"

// Please see trello task: https://trello.com/c/dwbflQHN

// For M1, no real fancy-schmancy AI pathfinding, just bare bones following 
// SOPHIA NOTE: THIS CURRENTLY DOES NOT WORK, WAITING ON GEETIKA'S PHYSICS SYSTEM
// WHICH WILL PROBABLY CAUSE THIS WHOLE THING TO CHANGE
void CreatureBehaviourSystem::follow_player(float elapsed_ms, Entity& follower)
{
	assert(registry.pathfinders.has(follower) && "The follower creature isn't registered as a pathfinder");
	Pathfinder& pathfinder_component = registry.pathfinders.get(follower);

	assert(registry.players.entities.size() == 1 && "There's no player for the creature to follow!");
	Entity& player = registry.players.entities[0];

	assert(registry.positions.has(player) && "Player doesn't have a motion component, so there's nothing for the creature to follow");
	assert(registry.positions.has(follower) && "Follower doesn't have a motion component!");

	// TODO: REFACTOR SO THIS IS GENERAL FOR ANY "TARGET". Whatever entity's ColourComponent triggers it
	// no trello task card yet because this will evolve with the dwbflQHN task
	const vec2& player_position = registry.positions.get(player);
	vec2& follower_position = registry.positions.get(follower);

	// Get the vector to the player
	float dx = player_position.x - follower_position.x;
	float dy = player_position.y - follower_position.y;

	// cheaper to avoid sqrt
	float distance_squared = dx * dx + dy * dy;
	float minDistToTarget_squared = pathfinder_component.minimumDistanceToTarget * pathfinder_component.minimumDistanceToTarget;

	// If too close to the target, stop
	if (distance_squared <= minDistToTarget_squared)
	{
		// followerVelocity = { 0.0f, 0.0f };
		return;
	}

	// Sqrt only when moving for normalization
	float distance = std::sqrt(distance_squared);
	float norm_x = dx / distance;
	float norm_y = dy / distance;

	float slow_radius = pathfinder_component.minimumDistanceToTarget * 3.f;
	float max_speed = pathfinder_component.max_speed;

	if (distance < slow_radius) {
		float t = (distance - pathfinder_component.minimumDistanceToTarget) / (slow_radius - pathfinder_component.minimumDistanceToTarget);
		t = std::clamp(t, 0.f, 1.f);
		max_speed = max_speed * t;
	} 
	// Should work but we have no velocity component right now
	//followerVelocity = { norm_x * max_speed, norm_y * max_speed };
	//std::cout << "follower velocity " << followerVelocity.x << ", " << followerVelocity.y << std::endl;
}