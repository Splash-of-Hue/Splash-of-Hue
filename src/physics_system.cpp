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

bool isCircleInRect(vec2 posC, float radiusC, vec2 point1, vec2 point2, vec2 point3)
{
	//math taken from https://stackoverflow.com/questions/2752725/finding-whether-a-point-lies-inside-a-rectangle-or-not
	vec2 AB = vec2{ point2.x - point1.x, point2.y - point1.y };
	vec2 AP = vec2{ posC.x - point1.x, posC.y - point1.y };
	vec2 BC = vec2{ point3.x - point2.x, point3.y - point2.y };
	vec2 BP = vec2{ posC.x - point2.x, posC.y - point2.y };

	//c++ does not like double less than comparisons, i think it acts as an OR or something?

	return (0 <= dot(AB, AP) && dot(AB, AP) <= dot(AB, AB)) && (0 <= dot(BC, BP) && dot(BC, BP) <= dot(BC, BC));
}

bool isLineInRangeOfCircle(vec2 posC, float radiusC, vec2 point1, vec2 point2)
{
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
	if (l2 == 0.0f)
	{
		return (hypot(AP.x, AP.y) <= radiusC);
	}
	else
	{
		float t = max(0.f, min(1.f, dot(AP, AB) / l2));
		vec2 projection = point1 + t * (AB);

		vec2 pProj = projection - posC;
		distance = hypot(pProj.x, pProj.y);

		return distance <= radiusC;
	}
}

bool collides(const Entity& entity1, const Entity& entity2)
{
	if (!registry.positions.has(entity1) || !registry.positions.has(entity2))
	{
		return false;
	}
	if (!registry.sizes.has(entity1) || !registry.sizes.has(entity2))
	{
		return false;
	}

	if (registry.circles.has(entity1) || registry.circles.has(entity2))
	{
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

		return (isCircleInRect(posC, radiusC, vertexTopLeft, vertexTopRight, vertexBottomRight) ||
				isLineInRangeOfCircle(posC, radiusC, vertexTopLeft, vertexTopRight) ||
				isLineInRangeOfCircle(posC, radiusC, vertexTopRight, vertexBottomRight) ||
				isLineInRangeOfCircle(posC, radiusC, vertexBottomRight, vertexBottomLeft) ||
				isLineInRangeOfCircle(posC, radiusC, vertexBottomLeft, vertexTopLeft));
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
	handle_movement(elapsed_ms);
	// check for collisions between all collidable entities
	ComponentContainer<Collidable>& collidables = registry.collidables;
	for (uint i = 0; i < collidables.components.size(); i++)
	{
		Entity& entity_i = collidables.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < collidables.components.size(); j++)
		{
			Entity& entity_j = collidables.entities[j];

			if (collides(entity_i, entity_j))
			{
				vec2& posI = registry.positions.get(entity_i);
				vec2& posJ = registry.positions.get(entity_j);

				//registry.list_all_components_of(entity_i);
				//std::cout << posI.x << " " << posI.y << std::endl;
				//registry.list_all_components_of(entity_j);
				//std::cout << posJ.x << " " << posJ.y << std::endl;

				// For now, this handles creature - circle interaction
				if (registry.colourStates.has(entity_i) && registry.colourStates.has(entity_j))
				{
					if (registry.colouredWall.has(entity_i) && registry.colouredWall.has(entity_j))
					{
						continue;
					}

					//colourcircles cannot interact with other colourcircles
					if (registry.colourCircles.has(entity_i) && registry.colourCircles.has(entity_j)) {
						goto after_color_collision;
					}
					
					const bool i_is_ColourCircle = registry.colourCircles.has(entity_i);
					const bool j_is_ColourCircle = registry.colourCircles.has(entity_j);

					const Entity& circle_entity = i_is_ColourCircle ? entity_i : entity_j;
					const Entity& other_entity = j_is_ColourCircle ? entity_j : entity_i;

					//colourcircles need to interact with creature? not sure if true anymore
					if (!registry.creatures.has(other_entity))
					{
						goto after_color_collision;
					}

					const ColourState& circle_color_state = registry.colourStates.get(circle_entity);
					ColourState& other_color_state = registry.colourStates.get(other_entity);

					// If the states are the same, move on
					if (circle_color_state.curr_colour_state == other_color_state.curr_colour_state)
					{
						goto after_color_collision;
					}

					other_color_state.prev_colour_state = other_color_state.curr_colour_state;
					other_color_state.curr_colour_state = circle_color_state.curr_colour_state;
					vec4& other_color = registry.colors.get(other_entity);
					const ColourPalette& palette = registry.colourPalettes.components[0];
					auto& color_from_state = palette.state_to_colour.at(other_color_state.curr_colour_state);
					other_color = color_from_state;
				}

				after_color_collision:
				//im wondering if its better to look at what type of creatures/entities we're dealing with BEFORE we look at their type
				//of collision. Because walls will be changing between inelastic and no collision, so im not entirely sure which
				//type of collision we want to put it in. For now, it will be its own thing.

				if (registry.colouredWall.has(entity_i) || registry.colouredWall.has(entity_j))
				{

					//find out which entity is which, since we need to find out if the other entity is a circle
					//although right now, im testing collisions between the hovering rectangle and a placed rectangle
					//so that does not really apply here
					Entity& eCW = registry.colouredWall.has(entity_i) ? entity_i : entity_j;
					Entity& eO = registry.colouredWall.has(entity_j) ? entity_i : entity_j;

					ColourState& csCW = registry.colourStates.get(eCW);
					ColourState& csO = registry.colourStates.get(eO);
					Collidable& colCW = registry.collidables.get(eCW);

					//if colourStates are same, wall is no longer inelastic, so player can walk through
					if (csCW.curr_colour_state == csO.curr_colour_state && csCW.curr_colour_state != COLOUR_STATE::NEUTRAL && registry.players.has(eO))
					{

						colCW.inelastic = false;
					}
					else
					{
						colCW.inelastic = true;
					}
				}

				//todo - change to sawfish once there are more creatures than sawfish
				if (registry.players.has(entity_i) && registry.creatures.has(entity_j) || registry.players.has(entity_j) && registry.creatures.has(entity_i)) {
					Entity& eP = registry.players.has(entity_i) ? entity_i : entity_j;
					Entity& eS = registry.creatures.has(entity_i) ? entity_i : entity_j;
					if (registry.colourStates.has(eS)) {
						ColourState& csS = registry.colourStates.get(eS);
						if (csS.curr_colour_state == COLOUR_STATE::RED) {
							if (!registry.deathTimers.has(eP)) {
								registry.deathTimers.emplace(eP);
							}
							
						}
					}
				}

				// TODO UurbFK4Z: Handle in callback instead of in the physics system
				if (registry.players.has(entity_i) && registry.spawnPoints.has(entity_j) || registry.players.has(entity_j) && registry.spawnPoints.has(entity_i))
				{
					Entity change_map_request_entity;
					Entity& spawn_point_entity = registry.spawnPoints.has(entity_i) ? entity_i : entity_j;
					const SpawnPoint& spawn_point = registry.spawnPoints.get(spawn_point_entity);
					registry.changeMapLevelRequests.insert(change_map_request_entity, { spawn_point.connected_level_uid, spawn_point.send_to_location });
				}

				//the button is a circle, is in an overlay? do we walk on top of the button?
				//is it only our radius that affects the button?
				//This assumed the button in a circle, and is an overlay, but since we send the rectangle
				//to "change_behaviours," i need to implement this code

				if (registry.toggles.has(entity_i) || registry.toggles.has(entity_j))
				{
					Entity& eT = registry.toggles.has(entity_i) ? entity_i : entity_j;
					Entity& eO = registry.toggles.has(entity_j) ? entity_i : entity_j;
					Toggle& tT = registry.toggles.get(eT);

					// Only the colourCircle can toggle things
					if (!registry.colourCircles.has(eO))
					{
						continue;
					}

					if (registry.colourStates.has(eO))
					{
						ColourState& csO = registry.colourStates.get(eO);

						// A neutral object does not toggle anything
						if (csO.curr_colour_state == COLOUR_STATE::NEUTRAL)
						{
							continue;
						}

						ColourState& csT = registry.colourStates.get(eT);

						vec4& colT = registry.colors.get(eT);
						vec4& colO = registry.colors.get(eO);
						colT = colO;

						for (Entity& toggledEntity : tT.walls_to_toggle)
						{
							ColourState& csTE = registry.colourStates.get(toggledEntity);
							csTE.prev_colour_state = csTE.curr_colour_state;
							csTE.curr_colour_state = csO.curr_colour_state;
							vec4& colTE = registry.colors.get(toggledEntity);
							colTE = vec4{ colO.r, colO.g, colO.b, colO.a };
						}
					}

				}
				else
				{
					if (registry.collidables.get(entity_i).overlay)
					{
						//let world_systems handle it
						//extra guard for potential errors
						if (!registry.change_behaviours.has(entity_j))
						{
							registry.change_behaviours.emplace(entity_j);
						}
					}
					if (registry.collidables.get(entity_j).overlay)
					{
						if (!registry.change_behaviours.has(entity_i))
						{
							registry.change_behaviours.emplace(entity_i);
						}
					}
					//must both be inelastic for this collision resolution to occur
					if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
					{
						//handle it here because using bounding box for correction (also it is a physics resolution)
						vec2& pos1 = registry.positions.get(entity_i);
						vec2& pos2 = registry.positions.get(entity_j);
						vec4 box1 = get_bounding_box(entity_i);
						vec4 box2 = get_bounding_box(entity_j);

						float x_depth = box1[3] < box2[3] ? box1[3] - box2[2] : box2[3] - box1[2];
						float y_depth = box1[1] < box2[1] ? box1[1] - box2[0] : box2[1] - box1[0];
						x_depth = fmaxf(0, x_depth);
						y_depth = fmaxf(0, y_depth);

						if (x_depth < y_depth)
						{
							if (pos1.x < pos2.x)
							{
								pos1.x -= x_depth;
							}
							else
							{
								pos1.x += x_depth;
							}

						}
						else
						{
							if (pos1.y < pos2.y)
							{
								pos1.y -= y_depth;
							}
							else
							{
								pos1.y += y_depth;
							}
						}

					}
				}
			}

		}
	}
}

void PhysicsSystem::handle_movement(float elapsed_ms)
{
	ComponentContainer<vec2>& moving_entities = registry.velocities;
	for (int i = 0; i < moving_entities.size(); i++)
	{
		vec2& pos = registry.positions.get(moving_entities.entities[i]);
		vec2& vel = registry.velocities.get(moving_entities.entities[i]);
		float step = elapsed_ms * ONE_THOUSANDTH;
		pos += (vel*step);
	}
}
