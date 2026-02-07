// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>
#include <math.h>
#include <numeric>

// Returns the local bounding coordinates scaled by the current size of the entity
vec4 get_bounding_box(const Entity& entity)
{
	Size& size = registry.sizes.get(entity);
	vec2& pos = registry.positions.get(entity);
	float half_height = size.height * 0.5f;
	float half_width = size.width * 0.5f;
	float bottom = pos[1] - half_height;
	float top = pos[1] + half_height;
	float left = pos[0] - half_width;
	float right = pos[0] + half_width;
	
	return { bottom, top, left, right };
}

bool isCircleInRect(vec2 posC, float radiusC, vec2 point1, vec2 point2, vec2 point3) {

	//math taken from https://stackoverflow.com/questions/2752725/finding-whether-a-point-lies-inside-a-rectangle-or-not
		
	vec2 AB = vec2{ point2.x - point1.x, point2.y - point1.y };
	vec2 AP = vec2{ posC.x - point1.x, posC.y - point1.y };
	vec2 BC = vec2{ point3.x - point2.x, point3.y - point2.y };
	vec2 BP = vec2{ posC.x - point2.x, posC.y - point2.y };

	//c++ does not like double less than comparisons, i think it acts as an OR or something?

	return (0 <= dot(AB, AP) && dot(AB, AP) <= dot(AB, AB)) && (0 <= dot(BC, BP) && dot(BC, BP) <= dot(BC, BC));

}

bool isLineInRangeOfCircle(vec2 posC, float radiusC, vec2 point1, vec2 point2) {
	//std::cout << "CirclePos: " << "{ " << posC.x << ", " << posC.y << " }" << std::endl;
	//std::cout << "Point1Pos: " << "{ " << point1.x << ", " << point2.y << " }" << std::endl;
	//std::cout << "Point2Pos: " << "{ " << point2.x << ", " << point2.y << " }" << std::endl;
	//calculation taken from https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection

	float distance = 0;
	vec2 AP = vec2(posC.x - point1.x, posC.y - point1.y);
	vec2 BP = vec2(posC.x - point2.x, posC.y - point2.y);
	vec2 AB = vec2(point2.x - point1.x, point2.y - point1.y);

	float l2 = pow(hypot(AB.x, AB.y), 2.f);

	//If the points are at the same location, then you're only checking if the radius reaches this one point
	if (l2 == 0.0f) {
		return (hypot(AP.x, AP.y) <= radiusC);
	}
	else {
		float t = max(0.f, min(1.f, dot(AP, AB) / l2));
		vec2 projection = point1 + t * (AB);

		vec2 pProj = projection - posC;
		distance = hypot(pProj.x, pProj.y);

		if (distance <= radiusC) {
			return true;
		}
		else {
			return false;
		}
	}
}


bool collides(const Entity& entity1, const Entity& entity2)
{
	if (!registry.positions.has(entity1) || !registry.positions.has(entity2)) {
		return false;
	}
	if (!registry.sizes.has(entity1) || !registry.sizes.has(entity2)) {
		return false;
	}
	
	if (registry.circles.has(entity1) || registry.circles.has(entity2)) {
		const Entity& eC = registry.circles.has(entity1) ? entity1 : entity2;
		const Entity& eR = registry.circles.has(entity1) ? entity2 : entity1;

		const vec2& posC = registry.positions.get(eC);
		const vec2& posR = registry.positions.get(eR);
		

		const Size& sC = registry.sizes.get(eC);
		const Size& sR = registry.sizes.get(eR);
		float radiusC = sR.width * 0.5f;

		float rectR = posR.x + sR.width * 0.5f;    //we're going to be doing a lot of comparing between
		float rectL = posR.x - sR.width * 0.5f;    //point on a circle, line on a rectangle, and radius of the circle
		float rectT = posR.y + sR.height * 0.5f;   //so this is just me creating variables to avoid calling things as much as possible
		float rectB = posR.y - sR.height * 0.5f;

		vec2 vertexTopLeft = { rectL, rectT };
		vec2 vertexTopRight = { rectR, rectT };
		vec2 vertexBottomLeft = { rectL, rectB };
		vec2 vertexBottomRight = { rectR, rectB };

		if (isCircleInRect(posC, radiusC, vertexTopLeft, vertexTopRight, vertexBottomRight) ||
			isLineInRangeOfCircle(posC, radiusC, vertexTopLeft, vertexTopRight) ||
			isLineInRangeOfCircle(posC, radiusC, vertexTopRight, vertexBottomRight) ||
			isLineInRangeOfCircle(posC, radiusC, vertexBottomRight, vertexBottomLeft) ||
			isLineInRangeOfCircle(posC, radiusC, vertexBottomLeft, vertexTopLeft))
		{
			return true;
		}
		else {
			return false;
		}
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
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		(void)elapsed_ms; // placeholder to silence unused warning until implemented
	}

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
