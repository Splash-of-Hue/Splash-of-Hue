// internal
#include "interactables/interactable_components.hpp"
#include "map/map_constants.hpp"
#include "physics_system.hpp"
#include "world_init.hpp"
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
	if ((registry.lightEmissions.has(entity_i) && registry.creatures.has(entity_j)) ||
		registry.lightEmissions.has(entity_j) && registry.creatures.has(entity_i))
	{
		return true;
	}

	if ((registry.lightEmissions.has(entity_i) && registry.mossToggles.has(entity_j)) ||
		registry.lightEmissions.has(entity_j) && registry.mossToggles.has(entity_i))
	{
		return true;
	}

	return false;
}


void light_emission_collisions(Entity& entity_i, Entity& entity_j)
{
	vec2& posI = registry.positions.get(entity_i);
	vec2& posJ = registry.positions.get(entity_j);

	Entity& light_emission_entity = registry.lightEmissions.has(entity_i) ? entity_i : entity_j;
	Entity& other_entity = registry.lightEmissions.has(entity_i) ? entity_j : entity_i;

	if (registry.creatures.has(other_entity))
	{
		if (registry.colourStates.has(other_entity))
		{
			COLOUR_STATE& colour_to_turn_into = registry.colourStates.get(light_emission_entity).curr_colour_state;

			LightEmission& light_emission_component = registry.lightEmissions.get(light_emission_entity);
			Entity& source_entity = light_emission_component.lightSourceEntity;

			if (registry.players.has(source_entity))
			{
				if (!registry.inPlayerRadius.has(other_entity))
				{
					registry.inPlayerRadius.emplace(other_entity);
				}

				InPlayerRadius& in_player_radius_component = registry.inPlayerRadius.get(other_entity);
				in_player_radius_component.colour = colour_to_turn_into;
				in_player_radius_component.radius_entity = light_emission_entity;
			}

			if (registry.creatures.has(source_entity))
			{
				if (!registry.inCreatureRadius.has(other_entity))
				{
					registry.inCreatureRadius.emplace(other_entity);
				}

				InCreatureRadius& in_creature_radius_component = registry.inCreatureRadius.get(other_entity);
				in_creature_radius_component.colour = colour_to_turn_into;
				in_creature_radius_component.radius_entity = light_emission_entity;
			}

			if (registry.interactables.has(source_entity))
			{
				if (!registry.inInteractableRadius.has(other_entity))
				{
					registry.inInteractableRadius.emplace(other_entity);
				}

				InInteractableRadius& in_interactable_radius_component = registry.inInteractableRadius.get(other_entity);
				in_interactable_radius_component.colour = colour_to_turn_into;
				in_interactable_radius_component.radius_entity = light_emission_entity;
			}
		}
	}

	//the button is a circle, is in an overlay? do we walk on top of the button?
	//is it only our radius that affects the button?
	//This assumed the button in a circle, and is an overlay, but since we send the rectangle
	//to "change_behaviours," i need to implement this code
	if (registry.mossToggles.has(entity_i) || registry.mossToggles.has(entity_j))
	{
		Entity& eT = registry.mossToggles.has(entity_i) ? entity_i : entity_j;
		Entity& eO = registry.mossToggles.has(entity_j) ? entity_i : entity_j;
		MossToggle& tT = registry.mossToggles.get(eT);

		// Only the player can toggle things
		if (!registry.lightEmissions.has(eO))
		{
			return;
		}

		if (registry.colourStates.has(eO))
		{
			ColourState& csO = registry.colourStates.get(eO);

			// A neutral object does not toggle anything
			if (csO.curr_colour_state == COLOUR_STATE::NEUTRAL)
			{
				return;
			}

			ColourState& csT = registry.colourStates.get(eT);

			vec4& colT = registry.colors.get(eT);
			vec4& colO = registry.colors.get(eO);
			colT = colO;
			csT.curr_colour_state = csO.curr_colour_state;

			for (std::string& connected_wall_id : tT.connected_wall_ids)
			{
				const auto wall_id_match = [&](const Entity& colored_wall_entity) { return registry.colouredWall.get(colored_wall_entity).id == connected_wall_id; };
				auto& wall_entity_iterator = std::find_if(registry.colouredWall.entities.begin(), registry.colouredWall.entities.end(), wall_id_match);

				if (wall_entity_iterator == registry.colouredWall.entities.end())
				{
					continue;
				}

				ColourState& csTE = registry.colourStates.get(*wall_entity_iterator);
				csTE.prev_colour_state = csTE.curr_colour_state;
				csTE.curr_colour_state = csO.curr_colour_state;
				vec4& colTE = registry.colors.get(*wall_entity_iterator);
				colTE = vec4{ colO.r, colO.g, colO.b, colO.a };
			}
		}

	}
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
		if (!registry.shrimps.has(eS))
		{
			if (registry.colourStates.has(eS))
			{
				ColourState& csS = registry.colourStates.get(eS);
				if (csS.curr_colour_state == COLOUR_STATE::RED)
				{
					if (registry.chargers.has(eS))
					{
						if (!registry.deathTimers.has(eP))
						{
							registry.deathTimers.emplace(eP);
						}
					}
				}
			}
		}
	}
}
void handle_misc_collision(Entity& entity_i, Entity& entity_j)
{
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

	if (registry.players.has(entity_j)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}

	if (!registry.velocities.has(entity_i)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}

	float x_depth = box1[3] < box2[3] ? box1[3] - box2[2] : box2[3] - box1[2];
	float y_depth = box1[1] < box2[1] ? box1[1] - box2[0] : box2[1] - box1[0];
	x_depth = fmaxf(0, x_depth);
	y_depth = fmaxf(0, y_depth);

	if (x_depth < y_depth)
	{
		if (pos_to_correct->x < other_pos->x)
		{
			pos_to_correct->x -= x_depth;
		}
		else
		{
			pos_to_correct->x += x_depth;
		}

	}
	else
	{
		if (pos_to_correct->y < other_pos->y)
		{
			pos_to_correct->y -= y_depth;
		}
		else
		{
			pos_to_correct->y += y_depth;
		}
	}
}

//mesh collision helpers
PolygonMesh findHitbox(const Entity& e)
{
	//default is rectangle

	vec2& pos = registry.positions.get(e);
	Size& size = registry.sizes.get(e);
	std::vector<vec2> vertices;
	//Sawfish heights and widths are too large, this is a temp fix
	//TODO: DMdDb4NW
	float half_width = size.width * 0.5f;
	float half_height = size.height * 0.5f;
	vertices.push_back({ vec2(pos.x - half_width , pos.y - half_height) });
	vertices.push_back({ vec2(pos.x + half_width, pos.y - half_height) }); // bottom
	vertices.push_back(vec2(pos.x + half_width, pos.y + half_height)); // top right
	vertices.push_back(vec2(pos.x - half_width, pos.y + half_height)); // top left
	PolygonMesh& polygon = PolygonMesh{ vertices, vertices, pos };

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

vec3 mesh_collides(const Entity& entity1, const Entity& entity2)
{
	PolygonMesh a;
	PolygonMesh b;
	if (registry.polygons.has(entity1)) {
		a = registry.polygons.get(entity1);
	}
	else
	{
		a = findHitbox(entity1);
	}
	if (registry.polygons.has(entity2)) {
		b = registry.polygons.get(entity2);
	}
	else
	{
		b = findHitbox(entity2);
	}
	return (axisCollision(a, b));
}

void handle_inelastic_collision_mesh(Entity& entity_i, Entity& entity_j, vec3 penetration)
{
	vec2& pos1 = registry.positions.get(entity_i);
	vec2& pos2 = registry.positions.get(entity_j);

	vec2* pos_to_correct = &pos1;
	vec2* other_pos = &pos2;

	if (registry.players.has(entity_j)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}

	if (!registry.velocities.has(entity_i)) {
		pos_to_correct = &pos2;
		other_pos = &pos1;
	}
	vec2 dir = *pos_to_correct - *other_pos;

	// if the normal points toward the other, flip
	float dir_check = dir.x * penetration.y + dir.y * penetration.z;
	float sign = dir_check >= 0 ? 1.0f : -1.0f;
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
		polygon.vertices[i] = polygon.centre + vec2{ rotated_x, rotated_y };
	}
}

void movePolygon(PolygonMesh& polygon, vec2& pos) {
	vec2 change_in_pos = pos - polygon.centre;
	if (change_in_pos.x == 0 && change_in_pos.y == 0) {
		return;
	}
	for (vec2& v : polygon.vertices) {
		v += change_in_pos;
	}
	for (vec2& v : polygon.base_vertices) {
		v += change_in_pos;
	}
	polygon.centre = pos;
}
void PhysicsSystem::step(float elapsed_ms)
{
	handle_movement(elapsed_ms);
	checkIfInRadius();

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

				if (registry.polygons.has(entity_i) || registry.polygons.has(entity_j))
				{
					vec3 penetration = mesh_collides(entity_i, entity_j);

					if (penetration.x != 0) {

						if (is_light_emission_collision(entity_i, entity_j))
						{
							light_emission_collisions(entity_i, entity_j);
						}
						else {

							handle_misc_collision(entity_i, entity_j);
						}

						if (registry.players.has(entity_i) && registry.colourOrbs.has(entity_j) ||
							registry.players.has(entity_j) && registry.colourOrbs.has(entity_i))
						{
							Entity& eOrb = registry.colourOrbs.has(entity_i) ? entity_i : entity_j;
							if (!registry.orbCollected.has(eOrb))
							{
								registry.orbCollected.emplace(eOrb);
							}
						}

						if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
						{
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
				else {
					handle_misc_collision(entity_i, entity_j);
				}
				//must both be inelastic for this collision resolution to occur
				if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
				{
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

		if (registry.polygons.has(e)) {
			movePolygon(registry.polygons.get(e), pos);
		}
		if (registry.angles.has(e)) {
			float radAngle = registry.angles.get(e).degrees * M_PI / 180.f;
			if (registry.polygons.has(e)) {
				rotatePolygon(registry.polygons.get(e), radAngle);
			}
		}
	}
}

void PhysicsSystem::checkIfInRadius()
{
	for (Entity& entity : registry.inPlayerRadius.entities)
	{
		InPlayerRadius& in_player_radius_component = registry.inPlayerRadius.get(entity);
		Entity& radius_entity = in_player_radius_component.radius_entity;
		if (registry.sizes.has(radius_entity))
		{
			if (!box_collides(entity, radius_entity))
			{
				registry.inPlayerRadius.remove(entity);
			}
		}
		else
		{
			registry.inPlayerRadius.remove(entity);
		}

	}

	for (Entity& entity : registry.inCreatureRadius.entities)
	{
		InCreatureRadius& in_creature_radius_component = registry.inCreatureRadius.get(entity);
		Entity& radius_entity = in_creature_radius_component.radius_entity;

		if (!box_collides(entity, radius_entity))
		{
			registry.inPlayerRadius.remove(entity);
		}
	}

	for (Entity& entity : registry.inInteractableRadius.entities)
	{
		InInteractableRadius& in_interactable_radius_component = registry.inInteractableRadius.get(entity);
		Entity& radius_entity = in_interactable_radius_component.radius_entity;

		if (!box_collides(entity, radius_entity))
		{
			registry.inPlayerRadius.remove(entity);
		}
	}
}