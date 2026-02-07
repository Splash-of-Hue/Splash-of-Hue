// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>

// Returns the local bounding coordinates scaled by the current size of the entity
vec4 get_bounding_box(const Entity& entity)
{
	Size& size = registry.sizes.get(entity);
	vec2& pos = registry.positions.get(entity);
	float half_height = size.height * 0.5;
	float half_width = size.width * 0.5;
	float bottom = pos[1] - half_height;
	float top = pos[1] + half_height;
	float left = pos[0] - half_width;
	float right = pos[0] + half_width;
	
	return { bottom, top, left, right };
}


bool collides(const Entity& entity1, const Entity& entity2)
{
	if (!registry.positions.has(entity1) || !registry.positions.has(entity2)) {
		return false;
	}
	if (!registry.sizes.has(entity1) || !registry.sizes.has(entity2)) {
		return false;
	}
	
	vec4 box1 = get_bounding_box(entity1);
	vec4 box2 = get_bounding_box(entity2);

	// This is the equivalent of (left1 > right2 OR right1 < left2) OR (bottom1 > top2 OR top1 < bottom2)
	// Which is asking if any of the edges of the boxes do NOT overlap.
	return ((box1[2] <= box2[3]) && 
			(box2[2] <= box1[3]) && 
			(box1[0] <= box2[1]) && 
			(box2[0] <= box1[1]));
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move each entity that has motion (invaders, projectiles, and even towers [they have 0 for velocity])
	// based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	//auto& motion_registry = registry.motions;
	//for(uint i = 0; i< motion_registry.size(); i++)
	//{
	//	
	//	//& motion = motion_registry.components[i];
	//	Entity entity = motion_registry.entities[i];
	//	float step_seconds = elapsed_ms / 1000.f;

	//	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	//}

	// check for collisions between all positioned entities
    ComponentContainer<vec2> &position_container = registry.positions;
	for(uint i = 0; i < position_container.components.size(); i++)
	{
		
		Entity& entity_i = position_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j < position_container.components.size(); j++)
		{
			Entity& entity_j = position_container.entities[j];
			if (collides(entity_i, entity_j))
			{
				
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				// CK: why the duplication, except to allow searching by entity_id
				if (registry.overlayCollisions.has(entity_i) || registry.overlayCollisions.has(entity_i)) {
					//let world_systems handle it
					registry.collisions.emplace(entity_i, entity_j);
				}
				//must both be inelastic for this collision resolution to occur
				if (registry.inelasticCollisions.has(entity_i) && registry.inelasticCollisions.has(entity_i)) {
					//handle it here because using bounding box for correction (also it is a physics resolution)
					vec2& pos1 = registry.positions.get(entity_i);
					vec2& pos2 = registry.positions.get(entity_j);
					vec4 box1 = get_bounding_box(entity_i);
					vec4 box2 = get_bounding_box(entity_j);

					
					float x_depth = box1[3] < box2[3] ? box1[3] - box2[2] : box2[3] - box1[2];
					float y_depth = box1[1] < box2[1] ? box1[1] - box2[0] : box2[1] - box1[0];

					//only correct one entity, TODO: correct moving entity pos
					if (x_depth < y_depth) {
						if (pos1.x < pos2.x) {
							pos1.x -= x_depth;
						}
						else {
							pos1.x += x_depth;
						}
						
					}
					else {
						if (pos1.y < pos2.y) {
							pos1.y -= y_depth;
						}
						else {
							pos1.y += y_depth;
						}
					}
					
				}

			}
			
		}
	}

	handle_player_movement();
}

// TODO SnB9A38K: Move to a player controller system and remove input in favour of component
void PhysicsSystem::handle_player_movement()
{
	auto &player_container = registry.players;
	assert(player_container.size() == 1 && "Should only be 1 player.");
	auto& player_entity = player_container.entities[0];

	auto& player_velocity = registry.motions.get(player_entity).velocity;
	std::cout << "DEBUG: Moving x: " << player_velocity.x << ", y: " << player_velocity.y << std::endl;
}
