// #include "tinyECS/components.hpp"
#include "tinyECS/registry.hpp"
#include <iomanip>		// for cout float decimal precision (std::fixed and std::setprecision)

const float MAX_X = 11;
//
// invader_movement_system
// - process all moving invaders
// - return number of entities moved, or 0 if no entities moved.
//
int invader_movement_system(Registry& entity_registry, float max_x)
{
	auto entities = entity_registry.get_entities();
	auto& exited = entity_registry.exited;
	auto& positions = entity_registry.positions;
	auto& moving = entity_registry.moving;
	int moved_count = 0;


	for (auto iter = entities.begin(); iter != entities.end(); ++iter)
    {
        auto& entity = **iter;

		if (!moving.has(entity))
		{
			continue;
		}

		
	// - increase their position.x by 1
		auto name = entity_registry.get_entity_name(entity);

		if (!entity_registry.has_component(entity, Position))
		{
			std::cout << "ERROR: Moving Entity " << name << " " << entity.id() << " does not have a position." << std::endl;
			continue;
		}

		auto position = positions.get(entity);
		PositionComponent old_position = position;
		position.x++;
		positions.update(entity, position);
		std::cout << "invader_movement_system: Moved Entity " << name << " " << entity.id() << " from (" << old_position.x << ", " << old_position.y << ") to (" << position.x << ", " << position.y << ")" << std::endl;
		//float max_x = 11;
		moved_count++;
		
		if (position.x < MAX_X)
		{
			continue;
		}
		
		// Don't count the entity once it has left and isn't detected
		moved_count--;
		moving.remove(entity);
		exited.insert(entity, *(new ExitedComponent()));
		std::cout << "invader_movement_system: Exited Entity " << name << " " << entity.id() << std::endl;
	}

	return moved_count;
}
