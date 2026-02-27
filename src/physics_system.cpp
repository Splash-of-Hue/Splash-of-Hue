// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <iostream>
#include <math.h>
#include <numeric>
#include "ecs/components.hpp";
#include <unordered_map>;

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

vec2 projection(ConvexPolygon& p, vec2 normal)
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
std::vector<vec2> getNormals(ConvexPolygon& p) {
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
vec3 axisCollision(ConvexPolygon& a, ConvexPolygon& b) {
	
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

vec3 collides(const Entity& entity1, const Entity& entity2)
{
	if (!registry.positions.has(entity1) || !registry.positions.has(entity2))
	{
		return vec3(0.f, 0.f, 0.f);
	}
	if (!registry.sizes.has(entity1) || !registry.sizes.has(entity2))
	{
		return vec3(0.f, 0.f, 0.f);
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

		if ((isCircleInRect(posC, radiusC, vertexTopLeft, vertexTopRight, vertexBottomRight) ||
			isLineInRangeOfCircle(posC, radiusC, vertexTopLeft, vertexTopRight) ||
			isLineInRangeOfCircle(posC, radiusC, vertexTopRight, vertexBottomRight) ||
			isLineInRangeOfCircle(posC, radiusC, vertexBottomRight, vertexBottomLeft) ||
			isLineInRangeOfCircle(posC, radiusC, vertexBottomLeft, vertexTopLeft))) {
			//since circle-rect collision prolly wont need penetration depth anyways
			//don't return 0 for x value though, otherwise it is "false"
			return vec3{ 1.0f, 0.0f, 0.0f };
		}
			
	}
	//create polygon either returns rectangle or predefined polygon of entity
	ConvexPolygon& a = createPolygon(entity1);
	ConvexPolygon& b = createPolygon(entity2);
	//std::cout << "a vertices: " << a.vertices.size() << " b vertices: " << b.vertices.size() << std::endl;
	return (axisCollision(a, b));
}
void handle_inelastic_collision(Entity entity_i, Entity entity_j, vec3 penetration) {
	vec2& pos1 = registry.positions.get(entity_i);
	vec2& pos2 = registry.positions.get(entity_j);
	vec4 box1 = get_bounding_box(entity_i);
	vec4 box2 = get_bounding_box(entity_j);

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
	pos_to_correct->x += (penetration.x * penetration.y*sign);
	pos_to_correct->y += (penetration.x * penetration.z*sign);
	
}

uint64_t hash_key(int tx, int ty) {
	//pack the tile x and y position into one 64 bit int for the key
	return  ((uint64_t)(uint32_t)tx) << 32 | (uint32_t)ty;
}
void PhysicsSystem::init() {
	grid.clear();

	int tx = WINDOW_WIDTH_PX / GRID_CELL_WIDTH_PX;
	int ty = WINDOW_HEIGHT_PX / GRID_CELL_HEIGHT_PX;
	for (int i = 0; i < tx; i++) {
		for (int j = 0; j < ty; j++) {
			//initialize grid to empty vector list
			grid[hash_key(i, j)] = std::vector<Entity>();
		}
	}
}
void PhysicsSystem::step(float elapsed_ms)
{
	if (!registry.changeMapLevelRequests.entities.empty()) {
		return;
	}
	handle_movement(elapsed_ms);
	std::set<uint64_t> checked;

	ComponentContainer<Collidable>& collidables = registry.collidables;
	for (int i = 0; i < collidables.components.size(); i++)
	{
		Entity& entity_i = collidables.entities[i];
		vec2& pos = registry.positions.get(entity_i);
		int tx = static_cast<int>(pos.x / GRID_CELL_WIDTH_PX);
		int ty = static_cast<int>(pos.y / GRID_CELL_HEIGHT_PX);

		//this loops over neighbouring grids for each entity, so the max amount of loops would be 9
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++) {
				std::unordered_map<uint64_t, std::vector<Entity>>::iterator tile = grid.find(hash_key(tx + i, ty + j)); 
				if (tile == grid.end()) {
					continue;
				}

				//loops over entities within that grid, which is considerably smaller amount than all existing entities
				for (Entity& entity_j : tile->second) {
					if (entity_j == entity_i) 
					   continue; // avoid self 

					//avoid duplicates:
					uint64_t pair = hash_key(entity_i.id(), entity_j.id());
					if (checked.count(pair)) 
						continue;
					checked.insert(pair);
					//if penetration is not 0, then a collision has occured
					vec3 penetration = collides(entity_i, entity_j);
					if (penetration.x != 0.f)
					{
						vec2& posI = registry.positions.get(entity_i);
						vec2& posJ = registry.positions.get(entity_j);

						// For now, this handles creature - player interaction
						if (registry.colourStates.has(entity_i) && registry.colourStates.has(entity_j))
						{
							if (registry.colouredWall.has(entity_i) && registry.colouredWall.has(entity_j))
							{
								continue;
							}

							// TODO: Determine color giving priority

							const bool i_is_player = registry.players.has(entity_i);
							const bool j_is_player = registry.players.has(entity_j);

							// For now, only a player can change states
							if (!i_is_player && !j_is_player)
							{
								goto after_color_collision;
							}

							const Entity& player_entity = i_is_player ? entity_i : entity_j;
							const Entity& other_entity = i_is_player ? entity_j : entity_i;

							if (!registry.creatures.has(other_entity))
							{
								goto after_color_collision;
							}

							const ColourState& player_color_state = registry.colourStates.get(player_entity);

							// A neutral player does not toggle anything
							if (player_color_state.curr_colour_state == COLOUR_STATE::NEUTRAL)
							{
								goto after_color_collision;
							}

							ColourState& other_color_state = registry.colourStates.get(other_entity);

							// If the states are the same, move on
							if (player_color_state.curr_colour_state == other_color_state.curr_colour_state)
							{
								goto after_color_collision;
							}

							other_color_state.prev_colour_state = other_color_state.curr_colour_state;
							other_color_state.curr_colour_state = player_color_state.curr_colour_state;
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

							//find out which entity is which, since we need to find out if the other entity is a player
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
							init();
							return;
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

							// Only the player can toggle things
							if (!registry.players.has(eO))
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
							//if (registry.collidables.get(entity_i).overlay)
							//{
							//	//let world_systems handle it
							//	//extra guard for potential errors
							//	if (!registry.change_behaviours.has(entity_j))
							//	{
							//		registry.change_behaviours.emplace(entity_j);
							//	}
							//}
							//if (registry.collidables.get(entity_j).overlay)
							//{
							//	if (!registry.change_behaviours.has(entity_i))
							//	{
							//		registry.change_behaviours.emplace(entity_i);
							//	}
							//}
						}

						if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
						{
							//handle it here because using bounding box for correction (also it is a physics resolution)
							handle_inelastic_collision(entity_i, entity_j, penetration);
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
		Entity& e = moving_entities.entities[i];
		vec2& pos = registry.positions.get(e);

		int old_tx = static_cast<int>(pos.x / GRID_CELL_WIDTH_PX);
		int old_ty = static_cast<int>(pos.y / GRID_CELL_HEIGHT_PX);

		vec2& vel = registry.velocities.get(e);
		float step = elapsed_ms*ONE_THOUSANDTH;
		pos += (vel*step);

		if (pos.x > WINDOW_WIDTH_PX || pos.x < 0)
		{
			pos.x = pos.x > WINDOW_WIDTH_PX ? WINDOW_WIDTH_PX : 0;
		}
		if (pos.y < 0 || pos.y > WINDOW_HEIGHT_PX)
		{
			pos.y = pos.y > WINDOW_HEIGHT_PX ? WINDOW_HEIGHT_PX : 0;
		}

		int tx = static_cast<int>(pos.x / GRID_CELL_WIDTH_PX);
		int ty = static_cast<int>(pos.y / GRID_CELL_HEIGHT_PX);

		//update the grid position of entity if it changed
		if (tx == old_tx && ty == old_ty) {
			continue;
		}

		//remove entity from other tile
		std::vector<Entity>& old_tile = grid.at(hash_key(old_tx, old_ty));
		old_tile.erase(std::remove(old_tile.begin(), old_tile.end(), e), old_tile.end());

		//push entity into the vector of the tile it is in
		grid.at(hash_key(tx, ty)).push_back(e);
	}
}
