// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
#include <iostream>

// todo: https://trello.com/c/D5ie5Ml8/120-task-score-task-template
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::aimAtTarget(Entity& charger, float elapsed_ms)
{

	Entity& player_entity = registry.players.entities[0];

	vec2& charger_position = registry.positions.get(charger);
	vec2& targeted_entity_position = registry.positions.get(player_entity);

	Angle& charger_angle = registry.angles.get(charger);

	float radAngle = atan((charger_position.x - targeted_entity_position.x) / (targeted_entity_position.y - charger_position.y));
	float degAngle = radAngle * 180 / M_PI;
	if (targeted_entity_position.y - charger_position.y < 0) {
		degAngle = -90 + degAngle;
	}
	else {
		degAngle = 90 + degAngle;
	}

	//rotate polygon mesh if change in degrees is non-negligible
	if (registry.polygons.has(charger) && abs(degAngle - charger_angle.degrees) > 10.f) {
		PolygonMesh& polygon = registry.polygons.get(charger);

		for (vec2& v : polygon.vertices) {
			vec2 dist_from_centre = v - polygon.centre;
			float c = cosf(radAngle);
			float s = sinf(radAngle);
			float rotated_x = dist_from_centre.x * c + dist_from_centre.y * s;
			float rotated_y = dist_from_centre.x * s - dist_from_centre.y * c;
			v = polygon.centre + vec2{ rotated_x, rotated_y };
		}
	}

	charger_angle.degrees = degAngle;
	
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::sawfishChargeAtTarget(Entity& charger, float elapsed_ms)
{
	Entity& targeted_entity = registry.players.entities[0];
	vec2& charger_position = registry.positions.get(charger);
	vec2& targeted_entity_position = registry.positions.get(targeted_entity);
	vec2 velocity = { targeted_entity_position.x - charger_position.x,targeted_entity_position.y - charger_position.y };
	double mag = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	velocity *= (1.f / mag);
	vec2& charger_velocity = registry.velocities.get(charger);
	charger_velocity = velocity * 1000.f;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::beDizzy(Entity& creature, float elapsed_ms)
{
	if (!registry.dizzies.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
	Angle& charger_angle = registry.angles.get(creature);
	charger_angle.degrees += 5.f;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}
