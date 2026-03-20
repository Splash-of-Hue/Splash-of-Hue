// internal
#include "interactables/interactable_components.hpp"
#include "map/map_constants.hpp"
#include "physics_system.hpp"
#include "shapes/shape_build_init.hpp"
#include <algorithm>
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

bool box_collides(const Entity& entity1, const Entity& entity2)
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
		float radiusC = sC.width * 0.5f;

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


//TODO gRCmzvrs - make more generalized once outside of tutorial
bool is_light_emission_collision(Entity& entity_i, Entity& entity_j)
{
	if ((registry.lightEmissions.has(entity_i) && registry.colourStates.has(entity_j)) ||
		registry.lightEmissions.has(entity_j) && registry.colourStates.has(entity_i))
	{
		return true;
	}

	return false;
}

// inBoundingBox is only applied to entities that need to check positional collisions
// and not a colour collision or light emission collision 
bool is_bounding_box_collision(Entity& entity_i, Entity& entity_j)
{
	if (registry.inBoundingBox.has(entity_i) && (registry.players.has(entity_j) || registry.creatures.has(entity_j)) ||
		registry.inBoundingBox.has(entity_j) && (registry.players.has(entity_i) || registry.creatures.has(entity_i)))
	{
		return true;
	}

	return false;
}

//todo: https://trello.com/c/tXEZLSr7/229-1-optimize-inradiuslist-in-physicssystemcpp
bool checkForRepeatedRadiuses(Entity& emission_entity, std::vector<Entity>& radius_list)
{
	for (Entity& receiving_entity : radius_list)
	{
		if (receiving_entity.id() == emission_entity.id())
		{
			return true;
		}
	}

	return false;
}

template <typename ComponentInterface>
void addSpecificRadiusCollision(Entity& entity_in_emission, Entity& emission_entity, ComponentInterface& container, bool& inRadius, std::vector<Entity>& radiusList)
{
	if (checkForRepeatedRadiuses(emission_entity, radiusList))
	{
		return;
	}

	LightEmission& light_emission_component = registry.lightEmissions.get(emission_entity);
	Entity& source_entity = light_emission_component.lightSourceEntity;

	if (container.has(source_entity))
	{
		inRadius = true;
		radiusList.push_back(emission_entity);
	}
}

void light_emission_collisions(Entity& entity_i, Entity& entity_j) 
{
	vec2& posI = registry.positions.get(entity_i);
	vec2& posJ = registry.positions.get(entity_j);

	Entity& light_emission_entity = registry.lightEmissions.has(entity_i) ? entity_i : entity_j;
	Entity& other_entity = registry.lightEmissions.has(entity_i) ? entity_j : entity_i;

	LightEmission& light_emission_component = registry.lightEmissions.get(light_emission_entity);
	if (light_emission_component.lightSourceEntity == other_entity)
	{
		return;
	}
	//These three conditions should cover everything we want and don't want to be affect by emissions. Keep an eye on this as more entities are created.
	if (registry.colourStates.has(other_entity) && !registry.lightEmissions.has(other_entity) && !registry.players.has(other_entity))
	{
		// Example: if its a player radius, set inPlayerRadius to true, and add the radius entity to playerRadius
		Entity& source_entity = registry.lightEmissions.get(light_emission_entity).lightSourceEntity;
		if (!registry.inRadius.has(other_entity))
			registry.inRadius.emplace(other_entity);
		InRadius& in_radius_component = registry.inRadius.get(other_entity);
		if (registry.players.has(source_entity))
			addSpecificRadiusCollision(other_entity, light_emission_entity, registry.players, in_radius_component.inPlayerRadius, in_radius_component.playerRadiusList);
		else if (registry.creatures.has(source_entity))
			addSpecificRadiusCollision(other_entity, light_emission_entity, registry.creatures, in_radius_component.inCreatureRadius, in_radius_component.creatureRadiusList);
		else if (registry.interactables.has(source_entity))
			addSpecificRadiusCollision(other_entity, light_emission_entity, registry.interactables, in_radius_component.inInteractableRadius, in_radius_component.interactableRadiusList);
	}
}

void bounding_box_collisions(Entity& entity_i, Entity& entity_j)
{
	vec2& posI = registry.positions.get(entity_i);
	vec2& posJ = registry.positions.get(entity_j);

	assert((!registry.inBoundingBox.has(entity_i) && !registry.inBoundingBox.has(entity_j)) && "bounding_box_collisions was called for an entity collision that does not have an inBoundingBox component");
	Entity& bounding_box_entity = registry.inBoundingBox.has(entity_i) ? entity_i : entity_j;
	Entity& other_entity = registry.inBoundingBox.has(entity_i) ? entity_j : entity_i;

	if (!registry.inBoundingBox.has(bounding_box_entity))
	{
		registry.inBoundingBox.emplace(bounding_box_entity);
	}

	InBoundingBox& inBoundingBox = registry.inBoundingBox.get(bounding_box_entity);

	inBoundingBox.isTouchingOtherEntity = true;
	inBoundingBox.updatedThisStep = true;
	
	// Unordered_set automatically guarantees no duplicates so no need to duplicate check! :)
	inBoundingBox.entityTouchingList.emplace(&other_entity); // see definition of entityTouchingList for reason behind inserting a reference
}

void handle_hurt_collision(Entity& entity_i, Entity& entity_j) {

	if (registry.creatures.has(entity_i) || registry.creatures.has(entity_j))
	{
		Entity& entity_creature = registry.creatures.has(entity_i) ? entity_i : entity_j;
		Entity& entity_other = registry.creatures.has(entity_j) ? entity_i : entity_j;

		if (!registry.creatureInelasticCollisions.has(entity_creature))
		{
			Collidable& other_collidable = registry.collidables.get(entity_other);
			//don't get dizzy if colliding with another creature
			if (other_collidable.inelastic)
			{
				registry.creatureInelasticCollisions.emplace(entity_creature);
				if (registry.breakables.has(entity_other))
				{
					registry.remove_all_components_of(entity_other);
				}
			}
		}
	}
	//todo - change to sawfish once there are more creatures than sawfish
	if (registry.players.has(entity_i) && registry.creatures.has(entity_j) || registry.players.has(entity_j) && registry.creatures.has(entity_i))
	{
		Entity& eP = registry.players.has(entity_i) ? entity_i : entity_j;
		Entity& eS = registry.creatures.has(entity_i) ? entity_i : entity_j;
		//make it so only sawfish kills
		if (!registry.shrimps.has(eS) && registry.colourStates.has(eS))
		{
			ColourState& csS = registry.colourStates.get(eS);
			
			if (csS.curr_colour_state == COLOUR_STATE::RED && registry.chargers.has(eS) && !registry.deathTimers.has(eP))
			{
				registry.deathTimers.emplace(eP);
			}
		}
	}
}
void handle_misc_collision(Entity& entity_i, Entity& entity_j)
{
	if (registry.players.has(entity_i) && registry.colourOrbs.has(entity_j) ||
		registry.players.has(entity_j) && registry.colourOrbs.has(entity_i))
	{
		Entity& eOrb = registry.colourOrbs.has(entity_i) ? entity_i : entity_j;
		if (!registry.orbCollected.has(eOrb))
		{
			registry.orbCollected.emplace(eOrb);
		}
	}

	if (registry.colouredWall.has(entity_i) || registry.colouredWall.has(entity_j))
	{
		Entity& eCW = registry.colouredWall.has(entity_i) ? entity_i : entity_j;
		Entity& eO = registry.colouredWall.has(entity_j) ? entity_i : entity_j;

		if (!registry.colourStates.has(eO))
		{
			return;
		}

		ColourState& csCW = registry.colourStates.get(eCW);
		ColourState& csO = registry.colourStates.get(eO);
		Collidable& colCW = registry.collidables.get(eCW);

		//if colourStates are same, wall is no longer inelastic, so player can walk through
		if (csCW.curr_colour_state == csO.curr_colour_state && csCW.curr_colour_state != COLOUR_STATE::NEUTRAL)
		{

			colCW.inelastic = false;
		}
		else
		{
			colCW.inelastic = true;
		}
	}

	// TODO UurbFK4Z: Handle in callback instead of in the physics system
	if (registry.teleporters.has(entity_j) || registry.teleporters.has(entity_i))
	{
		bool i_is_teleporter = registry.teleporters.has(entity_i);
		Entity& teleporter_entity = i_is_teleporter ? entity_i : entity_j;
		Entity& other_entity = i_is_teleporter ? entity_j : entity_i;

		if (!registry.currentLevels.has(other_entity))
		{
			// Only things that change levels can request to move
			return;
		}

		// Check if player or something else is requesting a movement
		bool is_player = registry.players.has(other_entity);
		MAP_CHANGE_REQUEST_TYPE map_change_request_type = is_player ? MAP_CHANGE_REQUEST_TYPE::PLAYER : MAP_CHANGE_REQUEST_TYPE::CREATURE;
		// Generate the request from the teleporter information
		const Teleporter& teleporter = registry.teleporters.get(teleporter_entity);
		Entity change_map_request_entity;
		ChangeMapLevelRequest level_change_request = {
			other_entity,
			teleporter.connected_level_uid,
			teleporter.arrival_location,
			map_change_request_type,
			teleporter.offset_direction
		};
		registry.changeMapLevelRequests.insert(change_map_request_entity, level_change_request);
	}
}

void handle_inelastic_collision_box(Entity& entity_i, Entity& entity_j)
{
	vec4 box1 = get_bounding_box(entity_i);
	vec4 box2 = get_bounding_box(entity_j);
	vec2& pos1 = registry.positions.get(entity_i);
	vec2& pos2 = registry.positions.get(entity_j);

	vec2* pos_to_correct = &pos1;
	vec2* other_pos = &pos2;

	if (!registry.velocities.has(entity_i)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}
	if (registry.velocities.has(entity_i) && registry.velocities.get(entity_i) == vec2(0)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}
	float x_depth = box1[3] < box2[3] ? box1[3] - box2[2] : box2[3] - box1[2];
	float y_depth = box1[1] < box2[1] ? box1[1] - box2[0] : box2[1] - box1[0];
	x_depth = fmaxf(0, x_depth);
	y_depth = fmaxf(0, y_depth);
	float dir_x = pos_to_correct->x < other_pos->x ? -1.f : 1.f;
	float dir_y = pos_to_correct->y < other_pos->y ? -1.f : 1.f;
	//if two things are moving and collide, split the correction and apply to both
	if (registry.velocities.has(entity_i) && registry.velocities.has(entity_j) && registry.velocities.get(entity_i) != vec2(0) && registry.velocities.get(entity_j) != vec2(0))
	{
		vec2 vel_i = registry.velocities.get(entity_i);
		vec2 vel_j = registry.velocities.get(entity_j);
		if (x_depth < y_depth) 
		{
			float x_correction = x_depth * dir_x * 0.5;
			//only correct if the entity was the one moving into the collision
			pos_to_correct->x += vel_i.x * x_correction >= 0.f ? 0.f : x_correction;
			other_pos->x -= vel_j.x * x_correction <= 0.f ? 0.f : x_correction;
		}
		else 
		{
			float y_correction = y_depth * dir_y * 0.5;
			pos_to_correct->y += vel_i.y * y_correction >= 0.f ? 0.f : y_correction;
			other_pos->y -= vel_j.y * y_correction <= 0.f ? 0.f : y_correction;
		}
		return;
	}

	if (x_depth < y_depth)
	{
		pos_to_correct->x += x_depth*dir_x;

	}
	else
	{
		pos_to_correct->y += y_depth*dir_y;
	}
}

//mesh collision helpers
PolygonMesh findHitbox(const Entity& e)
{
	//default is rectangle
	vec2& pos = registry.positions.get(e);
	Size& size = registry.sizes.get(e);
	std::vector<vec2> vertices;
	float half_width = size.width * 0.5f;
	float half_height = size.height * 0.5f;
	vertices.push_back({ vec2(pos.x - half_width , pos.y - half_height) });
	vertices.push_back({ vec2(pos.x + half_width, pos.y - half_height) }); // bottom
	vertices.push_back(vec2(pos.x + half_width, pos.y + half_height)); // top right
	vertices.push_back(vec2(pos.x - half_width, pos.y + half_height)); // top left
	PolygonMesh polygon = PolygonMesh{ vertices, vertices, pos };

	return polygon;
}
std::vector<vec2> getNormals(PolygonMesh& p) {
	std::vector<vec2> normals;
	//get first edge
	vec2 edge = p.vertices[0] - p.vertices[p.vertices.size() - 1];
	edge = vec2(edge.y, -edge.x);
	edge = normalize(edge);
	normals.push_back(edge);
	for (int i = 1; i < p.vertices.size(); i++) {
		edge = p.vertices[i] - p.vertices[i - 1];
		edge = vec2(edge.y, -edge.x);
		edge = normalize(edge);
		normals.push_back(edge);
	}
	return normals;
}

vec2 projection(PolygonMesh& p, vec2 normal)
{
	float min = dot(p.vertices[0], normal);
	float max = min;

	for (int i = 0; i < p.vertices.size(); i++) {
		float proj = dot(p.vertices[i], normal);
		min = std::min(proj, min);
		max = std::max(proj, max);
	}
	return vec2(min, max);
}

vec3 axisCollision(PolygonMesh& a, PolygonMesh& b)
{
	vec3 penetration = vec3(WINDOW_WIDTH_PX, 0.f, 0.f);
	std::vector<vec2> a_normals = getNormals(a);
	for (int i = 0; i < a_normals.size(); i++) {
		//project along each normal and if there is a gap, no collision
		vec2 proj_a = projection(a, a_normals[i]);
		vec2 proj_b = projection(b, a_normals[i]);

		//check if one projections max is less than the others min
		//if true, there is a gap
		if (proj_a.y < proj_b.x || proj_b.y < proj_a.x) {
			return vec3(0.f, 0.f, 0.f);
		}
		//keep track of smallest penetration for resolution
		float p = min(abs(proj_a.x - proj_b.y), abs(proj_a.y - proj_b.x));
		if (p < penetration.x) {
			penetration.x = p;
			penetration.y = a_normals[i].x;
			penetration.z = a_normals[i].y;
		}
	}
	std::vector<vec2> b_normals = getNormals(b);
	for (int i = 0; i < b_normals.size(); i++) {
		vec2 proj_a = projection(a, b_normals[i]);
		vec2 proj_b = projection(b, b_normals[i]);

		if (proj_a.y < proj_b.x || proj_b.y < proj_a.x) {
			return vec3(0.f, 0.f, 0.f);
		}
		float p = min(abs(proj_a.x - proj_b.y), abs(proj_a.y - proj_b.x));
		if (p < penetration.x) {
			penetration.x = p;
			penetration.y = b_normals[i].x;
			penetration.z = b_normals[i].y;
		}
	}
	return penetration;
}

vec3 mesh_collides(const Entity& entity1, const Entity& entity2,  bool isHurtCollision) 
{
	PolygonMesh entity1_mesh;
	PolygonMesh entity2_mesh;
	if (registry.hitboxes.has(entity1)) {
		entity1_mesh = registry.hitboxes.get(entity1);
	} else 
	{
		entity1_mesh = findHitbox(entity1);
	}
	if (registry.hitboxes.has(entity2)) {
		entity2_mesh = registry.hitboxes.get(entity2);
	}
	else
	{
		entity2_mesh = findHitbox(entity2);
	} 
	if (isHurtCollision) {
		if (registry.hurtboxes.has(entity1)) {
			entity1_mesh = registry.hurtboxes.get(entity1);
		}
		if (registry.hurtboxes.has(entity2)) {
			entity2_mesh = registry.hurtboxes.get(entity2);
		}
	}
	return (axisCollision(entity1_mesh, entity2_mesh));
}

void handle_inelastic_collision_mesh(Entity& entity_i, Entity& entity_j, vec3 penetration)
{
	vec2& pos1 = registry.positions.get(entity_i);
	vec2& pos2 = registry.positions.get(entity_j);

	vec2* pos_to_correct = &pos1;
	vec2* other_pos = &pos2;

	if (!registry.velocities.has(entity_i)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}
	if (registry.velocities.has(entity_i) && registry.velocities.get(entity_i) == vec2(0)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}
	vec2 dir = *pos_to_correct - *other_pos;
	// if the normal points toward the other, flip
	float dir_check = dir.x * penetration.y + dir.y * penetration.z;
	
	float sign = dir_check >= 0 ? 1.0f : -1.0f;

	//if two things are moving and collide, split the correction and apply to both

	if (registry.velocities.has(entity_i) && registry.velocities.has(entity_j) && registry.velocities.get(entity_i) != vec2(0) && registry.velocities.get(entity_j) != vec2(0))
	{
		vec2 vel_i = registry.velocities.get(entity_i);
		vec2 vel_j = registry.velocities.get(entity_j);
		float x_correction = penetration.x * penetration.y * sign * 0.5;
		//if moving into the collision, then it needs to be corrected
		pos_to_correct->x += vel_i.x * x_correction >= 0.f ? 0.f : x_correction;
		other_pos->x -= vel_j.x * x_correction <= 0.f ? 0.f : x_correction;

		float y_correction = penetration.x * penetration.z * sign * 0.5;
		pos_to_correct->y += vel_i.y * y_correction >= 0.f ? 0.f : y_correction;
		other_pos->y -= vel_j.y * y_correction <= 0.f ? 0.f : y_correction;
		return;
	}
	pos_to_correct->x += (penetration.x * penetration.y * sign);
	pos_to_correct->y += (penetration.x * penetration.z * sign);
}

void rotatePolygon(PolygonMesh& polygon, float radAngle) {
	float c = cosf(radAngle);
	float s = sinf(radAngle);
	for (int i = 0; i < polygon.vertices.size(); i++) {
		vec2 dist_from_centre = polygon.base_vertices[i] - polygon.centre;
		float rotated_x = dist_from_centre.x * c + dist_from_centre.y * s;
		float rotated_y = dist_from_centre.x * s - dist_from_centre.y * c;
		polygon.vertices[i] = polygon.centre + vec2( rotated_x, rotated_y );
	}
}

void movePolygon(PolygonMesh& polygon, vec2& pos) {
	vec2 change_in_pos = pos - polygon.centre;
	if (change_in_pos.x == 0 && change_in_pos.y == 0) {
		return;
	}
	for (int i = 0; i < polygon.vertices.size(); i++) {
		polygon.vertices[i] += change_in_pos;
		polygon.base_vertices[i] += change_in_pos;
	}
	polygon.centre = pos;
}

void PhysicsSystem::step(float elapsed_ms)
{
	handle_movement(DT);
	checkIfInRadius();
	checkIfInBoundingBox();

	// check for collisions between all collidable entities
	ComponentContainer<Collidable>& collidables = registry.collidables;
	for (uint i = 0; i < collidables.components.size(); i++)
	{
		Entity& entity_i = collidables.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < collidables.components.size(); j++)
		{
			Entity& entity_j = collidables.entities[j];

			//Two non_moving entities should not collide (unless one of them is a light source)
			if (!registry.velocities.has(entity_i) && !registry.velocities.has(entity_j)) {
				if (!registry.lightEmissions.has(entity_i) && !registry.lightEmissions.has(entity_j)) {
					continue;
				}
			}

			//if passes AABB check, then try mesh collision
			if (box_collides(entity_i, entity_j)) {
				if (registry.hurtboxes.has(entity_i) || registry.hurtboxes.has(entity_j))
				{
					vec3 penetration = mesh_collides(entity_i, entity_j, true);

					if (penetration.x != 0) {

						handle_hurt_collision(entity_i, entity_j);
						if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
						{
							handle_inelastic_collision_mesh(entity_i, entity_j, penetration);
						}
					}
				}

				if (registry.hitboxes.has(entity_i) || registry.hitboxes.has(entity_j))
				{
					vec3 penetration = mesh_collides(entity_i, entity_j, false);

					if (penetration.x != 0) {
						if (is_light_emission_collision(entity_i, entity_j))
						{
							light_emission_collisions(entity_i, entity_j);
						}
						else {
							handle_misc_collision(entity_i, entity_j);
							handle_hurt_collision(entity_i, entity_j);
						}

						if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
						{
							if (!registry.creatureInelasticCollisions.has(entity_j) && registry.creatures.has(entity_j)) {
								registry.creatureInelasticCollisions.emplace(entity_j);
							}
							if (!registry.creatureInelasticCollisions.has(entity_i) && registry.creatures.has(entity_i)) {
								registry.creatureInelasticCollisions.emplace(entity_i);
							}
							handle_inelastic_collision_mesh(entity_i, entity_j, penetration);
						}
						continue;
					}
					continue;
				}
			
				if (is_light_emission_collision(entity_i, entity_j))
				{
					light_emission_collisions(entity_i, entity_j);
				}
				else if (is_bounding_box_collision(entity_i, entity_j))
				{
					bounding_box_collisions(entity_i, entity_j);
				}
				else 
				{
					handle_misc_collision(entity_i, entity_j);
					handle_hurt_collision(entity_i, entity_j);
				}
				//must both be inelastic for this collision resolution to occur
				if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
				{
					if (!registry.creatureInelasticCollisions.has(entity_j) && registry.creatures.has(entity_j)) {
						registry.creatureInelasticCollisions.emplace(entity_j);
					}
					if (!registry.creatureInelasticCollisions.has(entity_i) && registry.creatures.has(entity_i)) {
						registry.creatureInelasticCollisions.emplace(entity_i);
					}
					handle_inelastic_collision_box(entity_i, entity_j);
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
		Entity& e = moving_entities.entities[i];
		vec2& pos = registry.positions.get(e);
		vec2& vel = registry.velocities.get(e);
		float step = elapsed_ms * ONE_THOUSANDTH;
		pos += (vel * step);

		if (registry.hitboxes.has(e)) {
			movePolygon(registry.hitboxes.get(e), pos);
		}
		if (registry.hurtboxes.has(e)) {
			movePolygon(registry.hurtboxes.get(e), pos);
		}
		if (registry.angles.has(e)) {
			float radAngle = registry.angles.get(e).degrees * M_PI / 180.f;
			if (registry.hitboxes.has(e)) {
				rotatePolygon(registry.hitboxes.get(e), radAngle);
			}
			if (registry.hurtboxes.has(e)) {
				rotatePolygon(registry.hurtboxes.get(e), radAngle);
			}
		}
	}
}

// If another entity is no longer touching this entity, remove it from its list
void PhysicsSystem::checkIfInBoundingBox()
{
	for (Entity& entity : registry.inBoundingBox.entities)
	{
		InBoundingBox& in_bounding_box_component = registry.inBoundingBox.get(entity);

		if (in_bounding_box_component.isTouchingOtherEntity = false)
		{
			continue;
		}

		checkIfColliding(entity, in_bounding_box_component);

		if (in_bounding_box_component.entityTouchingList.size() == 0)
		{
			in_bounding_box_component.isTouchingOtherEntity = false;
		}
	}
}

void PhysicsSystem::checkIfColliding(Entity& entity, InBoundingBox& in_bounding_box_component) // Pass in entity and component to avoid additional registry.inBoundingBox.gets as we use both
{
	std::unordered_set<Entity*>& touching_list = in_bounding_box_component.entityTouchingList;
	bool& updatedThisStep = in_bounding_box_component.updatedThisStep;

	for (auto it = touching_list.begin(); it != touching_list.end();)
	{
		const Entity* touching_entity = *it;

		if (!registry.sizes.has(*touching_entity) || !box_collides(entity, *touching_entity))
		{
			it = touching_list.erase(it);
			updatedThisStep = true;
		}
		else
		{
			++it;
		}
	}

	return;
}

void PhysicsSystem::checkIfInRadius()
{
	for (Entity& entity : registry.inRadius.entities)
	{
		InRadius& in_radius_component = registry.inRadius.get(entity);
		if (in_radius_component.inPlayerRadius)
		{
			checkIfCollidingRadius(entity, in_radius_component.playerRadiusList);
			if (in_radius_component.playerRadiusList.size() == 0)
			{
				in_radius_component.inPlayerRadius = false;
			}
		}
		if (in_radius_component.inCreatureRadius)
		{
			checkIfCollidingRadius(entity, in_radius_component.creatureRadiusList);
			if (in_radius_component.creatureRadiusList.size() == 0)
			{
				in_radius_component.inCreatureRadius = false;
			}
		}
		if (in_radius_component.inInteractableRadius)
		{
			checkIfCollidingRadius(entity, in_radius_component.interactableRadiusList);
			if (in_radius_component.interactableRadiusList.size() == 0)
			{
				in_radius_component.inInteractableRadius = false;
			}
		}

		if (!in_radius_component.inPlayerRadius && !in_radius_component.inCreatureRadius && !in_radius_component.inInteractableRadius)
		{
			registry.inRadius.remove(entity);	
		}
	}
}

void PhysicsSystem::checkIfCollidingRadius(Entity& entity, std::vector<Entity>& radius_list)
{
	std::vector<int> indicesToErase;
	int indexToErase = 0;
	for (Entity& radius_entity : radius_list)
	{
		if (!registry.sizes.has(radius_entity) || !box_collides(entity, radius_entity))
		{
			indicesToErase.push_back(indexToErase);
		}
	}
	
	for (int index : indicesToErase)
	{
		radius_list.erase(radius_list.begin() + index);
	}
	
	return;
}
