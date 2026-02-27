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
bool axisCollision(ConvexPolygon& a, ConvexPolygon& b) {
	return false;
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
void handle_inelastic_collision(Entity entity_i, Entity entity_j) {
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

uint64_t hash_key(int tx, int ty) {
	//pack the tile x and y position into one 64 bit int for the key
	return  ((uint64_t)(uint32_t)tx) << 32 | (uint32_t)ty;
}
void PhysicsSystem::step(float elapsed_ms)
{
	handle_movement(elapsed_ms);

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
				std::unordered_map<uint64_t, std::vector<Entity>>::iterator tile = grid.find(hash_key(tx + i, ty + j)); tile = grid.find(hash_key(tx + i, ty + j));
				if (tile == grid.end()) {
					continue;
				}

				//loops over entities within that grid, which is considerably smaller amount than all existing entities
				for (Entity& entity_j : tile->second) {
					if (entity_j <= entity_i) 
					   continue; // avoid duplicates
					if (collides(entity_i, entity_j))
					{
						if (registry.collidables.get(entity_i).inelastic && registry.collidables.get(entity_j).inelastic)
						{
							//handle it here because using bounding box for correction (also it is a physics resolution)
							handle_inelastic_collision(entity_i, entity_j);
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
		std::vector<Entity>& old_tile = grid[hash_key(old_tx, old_ty)];
		old_tile.erase(std::remove(old_tile.begin(), old_tile.end(), e), old_tile.end());

		//push entity into the vector of the tile it is in
		grid[hash_key(tx, ty)].push_back(e);
	}
}
