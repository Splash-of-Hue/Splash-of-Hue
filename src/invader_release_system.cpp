// #include "tinyECS/components.hpp"
#include "tinyECS/registry.hpp"

const float MIN_X = 0;
const float MAX_X = 11;
const float MIN_Y = 0;
const float MAX_Y = 9;
//
// invader_release_system
// - process all queued invaders
// - return number of entities queued, or 0 if no entities queued
//
int invader_release_system(Registry& entity_registry)
{
	auto entities = entity_registry.get_entities();
	auto& invaders = entity_registry.invaders;
	auto& queued = entity_registry.queued;
	auto& release_timers = entity_registry.release_timers;
	auto& positions = entity_registry.positions;
	std::vector<Entity*> releasing_entities = {};

	for (auto iter = entities.begin(); iter != entities.end(); ++iter)
    {
		auto& entity = **iter;

		if (!queued.has(entity))
		{
			continue;
		}

		auto name = entity_registry.get_entity_name(entity);

		if (!entity_registry.has_component(entity, ReleaseTimer))
		{
			std::cout << "ERROR: Queued Entity " << name << " " << entity.id() << " does not have a release_timer." << std::endl;
		}

		auto release_timer = release_timers.get(entity);
		float previous_time = release_timer.timer;
		release_timer.timer--;
		release_timers.update(entity, release_timer);

		std::cout << "invader_release_system: Entity " << name << " " << entity.id() << " release timer change from " << previous_time << " to " << release_timer.timer << std::endl;
 
		if (release_timer.timer > 0)
		{
			continue;
		}

		releasing_entities.push_back(&entity);
    }

	for (auto i = 0; i < releasing_entities.size(); i++)
	{
		auto& entity = *releasing_entities[i];
		auto name = entity_registry.get_entity_name(entity);

		queued.remove(entity);
		release_timers.remove(entity);

		// If it doesn't have a position, can't do anything
		if (!entity_registry.has_component(entity, Position))
		{
			std::cout << "ERROR: Queued Entity " << name << " " << entity.id() << " does not have a valid position." << std::endl;
			continue;
		}

		auto position = positions.get(entity);

		//   - if they have a valid X and Y position then
		if ((position.x < MIN_X || position.x > MAX_X || position.y < MIN_Y || position.y > MAX_Y))
		{
			// Invalid coordinates
			std::cout << "ERROR: Queued Entity " << name << " " << entity.id() << " does not have a valid position." << std::endl;
			continue;
		}

		entity_registry.moving.insert(entity, *(new MovingComponent()));
		std::cout << "invader_release_system: Releasing Entity " << name << " " << entity.id() << " at position(" << position.x << ", " << position.y << ")" << std::endl;
	}

	releasing_entities.clear();

	// - loop through all 'queued' entities and decrease their release timer by 1
	// - report all release_timers
	//   - print "invader_release_system: Entity <entity_name> <entity_id> release timer change from <current_timer_value> to <new_timer_value>
	// - queued entity does not have a release_timer
	//   - print "ERROR: Queued Entity <entity_name> <entity_id> does not have a release_timer."
	// - when their release timer reaches zero:
	//   - remove their 'queued' component
	//   - remove their 'release_timer' component
	//   - if they have a valid X and Y position then
	//     - add a new 'moving' component
	//     - print "Releasing Entity <entity-name> <entity-id> at position (<x-position>, <y-position>)"
	//   - if they DO NOT have a valid X and Y position then
	//     - print "ERROR: Queued Entity <entity-name> <entity-id> does not have a valid position."
	// 

	// TODO: return the number of entities still queued
	return entity_registry.queued.size();
}