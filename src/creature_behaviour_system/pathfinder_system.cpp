// stlib
#include <iostream>
#include <sstream>

#include "common.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

// For M1, no real fancy-schmancy AI pathfinding, just bare bones following 
void CreatureBehaviourSystem::follow_player(float elapsed_ms, Entity& follower)
{
	assert(registry.pathfinders.has(follower) && "The follower creature isn't registered as a pathfinder");
	Pathfinder& pathfinder_component = registry.pathfinders.get(follower);

	assert(registry.players.entities.size() == 1 && "There's no player for the creature to follow!");
	Entity& player = registry.players.entities[0];

	assert(registry.motions.has(player) && "Player doesn't have a motion component, so there's nothing for the creature to follow");
	assert(registry.motions.has(follower) && "Follower doesn't have a motion component!");

	// TODO: REFACTOR SO THIS IS GENERAL FOR ANY "TARGET". Whatever entity's ColourComponent triggers it
	const Motion& player_motion_component = registry.motions.get(player);
	Motion& follower_motion_component = registry.motions.get(follower);

	// TODO: REFACTOR ONCE "POSITION" COMPONENT IS IN PLAY
	const vec2 playerPosition = player_motion_component.position;
	vec2 followerPosition = follower_motion_component.position;

	vec2 followerVelocity = follower_motion_component.velocity;

	// Get the vector to the player
	float dx = playerPosition.x - followerPosition.x;
	float dy = playerPosition.y - followerPosition.y;

	// cheaper to avoid sqrt
	float distance_squared = dx * dx + dy * dy;
	float minDistToTarget_squared = pathfinder_component.minimumDistanceToTarget * pathfinder_component.minimumDistanceToTarget;

	// If too close to the target, stop
	if (distance_squared <= minDistToTarget_squared)
	{
		followerVelocity.x = 0.0f;
		followerVelocity.y = 0.0f;
		return;
	}

	// Sqrt only when moving for normalization
	float distance = std::sqrt(distance_squared);
	float norm_x = dx / distance;
	float norm_y = dy / distance;

	float max_speed = pathfinder_component.max_speed;
	followerVelocity = { norm_x * max_speed, norm_y * max_speed };
	followerPosition = { followerPosition.x + followerVelocity.x * elapsed_ms, followerPosition.y + followerVelocity.y * elapsed_ms };
	
}