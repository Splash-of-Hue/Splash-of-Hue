// #include "tinyECS/components.hpp"
#include "tinyECS/registry.hpp"
#include <iomanip>		// for cout float decimal precision (std::fixed and std::setprecision)
#include <set>

//
// invader_detection_system
// - determine which invaders are detected by towers
// - return number of entities detected by the towers, or 0 if no entities detected.
//
int invader_detection_system(Registry& entity_registry)
{
	auto entities = entity_registry.get_entities();
	auto& invaders = entity_registry.invaders;
	auto& positions = entity_registry.positions;
	auto& moving = entity_registry.moving;
	auto& towers = entity_registry.towers;
	auto& detected = entity_registry.detected;
	auto& exited = entity_registry.exited;
	std::set<float> covered_lanes = {};


	for (auto iter = entities.begin(); iter != entities.end(); ++iter)
    {
        auto& entity = **iter;

		if (!towers.has(entity) || !positions.has(entity))
		{
			continue;
		}

		auto position = positions.get(entity);
		auto tower = towers.get(entity);

		auto upper_range = position.y;
		auto lower_range = position.y;

		if (tower.range > 0)
		{
			upper_range += tower.range - 1;
			lower_range -= tower.range - 1;
		}

		for (auto i = lower_range; i <= upper_range; i++)
		{
			covered_lanes.insert(i);
		}
	}


	for (auto iter = entities.begin(); iter != entities.end(); ++iter)
    {
        auto& entity = **iter;
		auto name = entity_registry.get_entity_name(entity);
		bool is_detected = detected.has(entity);
		bool has_exited = exited.has(entity);

		if (has_exited && is_detected)
		{
			detected.remove(entity);
			continue;
		}

		if (!moving.has(entity) || !invaders.has(entity))
		{
			continue;
		}
		
		if (!positions.has(entity))
		{
			std::cout << "ERROR: Moving Entity " << name << " " << entity.id() << " does not have a position." << std::endl;
			continue;
		}
		
		auto position = positions.get(entity);
		float min_x = 0;
		float max_x = 11;
		auto search = covered_lanes.find(position.y);
		bool has_valid_detection_position = search != covered_lanes.end() && (position.x > min_x && position.x < max_x);
		
		if (!is_detected && has_valid_detection_position)
		{
			std::cout << "invader_detection_system: Detected Entity " << name << " " << entity.id() << " at (" << position.x << ", " << position.y << ")" << std::endl;
			detected.insert(entity, *(new DetectedComponent()));
			continue;
		}

		if (is_detected && !has_valid_detection_position)
		{
			
			std::cout << "invader_detection_system: No longer detecting Entity " << name << " " << entity.id() << " at (" << position.x << ", " << position.y << ")" << std::endl;
			detected.remove(entity);
			continue;
		}
	}

	// TODO:
	// for each moving invader
	// - mark all detected invaders as "detected"
	//   - print "invader_detection_system: Detected Entity <invader-name> <invader-id> at (<invader-x-position>, <invader-y-position>)"
	// - remove "detected" from any invaders no longer detected
	//   - print "invader_detection_system: No longer detecting Entity <invader-name> <invader-id> at (<invader-x-position>, <invader-y-position>)"
	// - if the invader is 'moving' but does not have a position:
	//   - print "ERROR: Moving Entity <entity-name> <entity-id> does not have a position."

	// TODO: return the number of detected entities, or zero if no entities detected or still detected
	return entity_registry.detected.size();
}
