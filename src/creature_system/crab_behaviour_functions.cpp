// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::slideForwards(Entity& creature, float elapsed_ms)
{
	vec2& velocity = registry.velocities.get(creature);
	vec2& direction = registry.directions.get(creature);

	velocity = direction * PLAYER_SPEED;

    ANIMATION_TRIGGER trigger = ANIMATION_TRIGGER::WALK_FORWARD;
    if (direction.x > 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_RIGHT;
    }
    else if (direction.x < 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_LEFT;
    }
    else if (direction.y > 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_FORWARD;
    }
    else if (direction.y < 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_BACKWARD;
    }

    if (registry.animation_triggers.has(creature)) {
        registry.animation_triggers.get(creature).animation_trigger = trigger;
    }
    else {
        registry.animation_triggers.insert(creature, { trigger });
    }

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::slideBackwards(Entity& creature, float elapsed_ms)
{
	vec2& velocity = registry.velocities.get(creature);
	vec2& direction = registry.directions.get(creature);
	velocity = -direction * PLAYER_SPEED;

    ANIMATION_TRIGGER trigger = ANIMATION_TRIGGER::WALK_BACKWARD;
    if (direction.x > 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_LEFT;
    }
    else if (direction.x < 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_RIGHT;
    }
    else if (direction.y > 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_BACKWARD;
    }
    else if (direction.y < 0.f) {
        trigger = ANIMATION_TRIGGER::WALK_FORWARD;
    }

    if (registry.animation_triggers.has(creature)) {
        registry.animation_triggers.get(creature).animation_trigger = trigger;
    }
    else {
        registry.animation_triggers.insert(creature, { trigger });
    }

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

// ====== Decorator Nodes ======
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isSliding(Entity& creature, float elapsed_ms)
{
	if (registry.velocities.get(creature) != vec2(0)) {
		return BEHAVIOUR_TREE_STATUS::SUCCESS;
	}
    if (registry.animation_triggers.has(creature)) {
        registry.animation_triggers.get(creature).animation_trigger = ANIMATION_TRIGGER::IDLE;
    }
    else {
        registry.animation_triggers.insert(creature, { ANIMATION_TRIGGER::IDLE });
    }
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}
