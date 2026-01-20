#pragma once
#include <iostream>
// #include <unordered_set>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <typeinfo>
#include <typeindex>
#include "trace.h"
#include "tiny_ecs.hpp"
#include "entity.hpp"
#include "components.hpp"

// Component registry
class Registry
{
private:
	std::vector<Entity*> entities_ = std::vector<Entity*>();
	std::map<unsigned int, size_t> entity_lookup_ = std::map<unsigned int, size_t>();
	std::map<unsigned int, std::string> entity_names_ = std::map<unsigned int, std::string>();
	std::map<std::string, unsigned int> entity_names_reverse_ = std::map<std::string, unsigned int>();
	std::map<unsigned int, ComponentType> entity_components_ = std::map<unsigned int, ComponentType>();

public:
	// Component containers
	ComponentContainer<DetectedComponent> detected = ComponentContainer<DetectedComponent>();
	ComponentContainer<ExitedComponent> exited = ComponentContainer<ExitedComponent>();
	ComponentContainer<InvaderComponent> invaders = ComponentContainer<InvaderComponent>();
	ComponentContainer<MovingComponent> moving = ComponentContainer<MovingComponent>();
	ComponentContainer<PositionComponent> positions = ComponentContainer<PositionComponent>();
	ComponentContainer<QueuedComponent> queued = ComponentContainer<QueuedComponent>();
	ComponentContainer<ReleaseTimerComponent> release_timers = ComponentContainer<ReleaseTimerComponent>();
	ComponentContainer<TowerComponent> towers = ComponentContainer<TowerComponent>();

	Registry() {}

	// Create a unique Entity to be associated with Components
	Entity create_entity() {
		auto new_entity = new Entity();
		entities_.push_back(new_entity);
		auto entity_id = (*new_entity).id();
		entity_lookup_[entity_id] = entities_.size() - 1;

		return *new_entity;
	}

	// TODO: Find better method after getting functionality down
	std::vector<Entity*> get_entities()
	{
		return entities_;
	}

	bool remove_entity(Entity entity)
	{
		// Should have checked if entity existed by name, but for safety, check existance
		auto search = entity_names_.find(entity.id());

    	if (search == entity_names_.end())
		{
			return false;
		}

		if (has_component(entity, Detected))
		{
			detected.remove(entity);
		}
		if (has_component(entity, Invader))
		{
			invaders.remove(entity);
		}
		if (has_component(entity, Moving))
		{
			moving.remove(entity);
		}
		if (has_component(entity, Position))
		{
			positions.remove(entity);
		}
		if (has_component(entity, Queued))
		{
			queued.remove(entity);
		}
		if (has_component(entity, ReleaseTimer))
		{
			release_timers.remove(entity);
		}
		if (has_component(entity, Tower))
		{
			towers.remove(entity);
		}

		entity_components_.erase(entity);
		auto& entity_name = entity_names_[entity];
		entity_names_reverse_.erase(entity_name);
		entity_names_.erase(entity);
		auto& entity_position = entity_lookup_[entity];
		auto end_entity_position = entities_.size() - 1;
		auto end_entity = entities_[end_entity_position];
		entities_.pop_back();

		// If the thing being removed is not the last on the list OR the last remaining, swap the last to that spot
		if (entity_position != end_entity_position)
		{
			entities_[entity_position] = end_entity;
			auto end_entity_id = (*end_entity).id();
			entity_lookup_[end_entity_id] = entity_position;
		}

		return true;
	}

	float get_component_value(Entity entity, ComponentType component_type, std::string member_name)
	{
		// Currently, only support a limited amount of input, so hardcode for now
		bool is_position = component_type & Position;
		bool is_release_timer = component_type & ReleaseTimer;
		bool is_tower = component_type & Tower;

		if (!is_position && !is_release_timer && !is_tower && !has_component(entity, component_type))
		{
			// Poor error handling for now
			return 0;
		}

		if (is_tower && member_name == "range")
		{
			auto tower = towers.get(entity);
			return tower.range;
		}

		if (is_release_timer && member_name == "timer")
		{
			auto release_timer = release_timers.get(entity);
			return release_timer.timer;
		}

		if (is_position && member_name == "x")
		{
			auto position = positions.get(entity);
			return position.x;
		}

		if (is_position && member_name == "y")
		{
			auto position = positions.get(entity);
			return position.y;
		}

		return 0;
	}

	bool set_component_value(Entity entity, ComponentType component_type, std::string member_name, float value)
	{
		// Currently, only support a limited amount of input, so hardcode for now
		bool is_position = component_type & Position;
		bool is_release_timer = component_type & ReleaseTimer;
		bool is_tower = component_type & Tower;

		if (!is_position && !is_release_timer && !is_tower)
		{
			// None of the valid options
			return false;
		}

		if (is_tower && member_name == "range")
		{
			auto tower = towers.get(entity);
			tower.range = value;
			towers.update(entity, tower);
			return true;
		}

		if (is_release_timer && member_name == "timer")
		{
			auto release_timer = release_timers.get(entity);
			release_timer.timer = value;
			release_timers.update(entity, release_timer);
			return true;
		}

		if (is_position && member_name == "x")
		{
			auto position = positions.get(entity);
			position.x = value;
			positions.update(entity, position);
			return true;
		}

		if (is_position && member_name == "y")
		{
			auto position = positions.get(entity);
			position.y = value;
			positions.update(entity, position);
			return true;
		}

		return false;
	}

	void name_entity(Entity entity, std::string name)
	{
		entity_names_[entity.id()] = name;
		entity_names_reverse_[name] = entity;
	}

	std::string get_entity_name(Entity entity)
	{
		return entity_names_[entity.id()];
	}
	
	bool has_entity_by_name(std::string entity_name)
	{
		auto search = entity_names_reverse_.find(entity_name);
		return search != entity_names_reverse_.end();
	}


	Entity get_entity_by_name(std::string entity_name)
	{
		auto entity_id = entity_names_reverse_[entity_name];
		auto entity_lookup = entity_lookup_[entity_id];

		return *entities_[entity_lookup];
	}

	bool has_component(Entity entity, ComponentType component_type)
	{
		auto component_flags = entity_components_[entity.id()];
		return (component_flags & component_type) > 0;
	}
	
	bool add_component(Entity entity, ComponentType component_type)
	{
		switch (component_type)
		{
			case Invalid:
				return false;
			case Detected:
				detected.insert(entity, DetectedComponent());
				break;
			case Exited:
				exited.insert(entity, ExitedComponent());
				break;
			case Invader:
				invaders.insert(entity, InvaderComponent());
				break;
			case Moving:
				moving.insert(entity, MovingComponent());
				break;
			case Position:
				positions.insert(entity, PositionComponent());
				break;
			case Queued:
				queued.insert(entity, QueuedComponent());
				break;
			case ReleaseTimer:
				release_timers.insert(entity, ReleaseTimerComponent());
				break;
			case Tower:
				towers.insert(entity, TowerComponent());
				break;
		}
		
		entity_components_[entity.id()] = entity_components_[entity.id()] | component_type;
		return true;
	}


	bool remove_component(Entity entity, ComponentType component_type)
	{
		switch (component_type)
		{
		case Invalid:
			return false;
		case Detected:
			detected.remove(entity);
			break;
		case Exited:
			exited.remove(entity);
			break;
		case Invader:
			invaders.remove(entity);
			break;
		case Moving:
			moving.remove(entity);
			break;
		case Position:
			positions.remove(entity);
			break;
		case Queued:
			queued.remove(entity);
			break;
		case ReleaseTimer:
			release_timers.remove(entity);
			break;
		case Tower:
			towers.remove(entity);
			break;
		}

		entity_components_[entity.id()] = entity_components_[entity.id()] & static_cast<ComponentType>(~component_type);
		return true;
	}
};
