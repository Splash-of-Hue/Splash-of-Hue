#include <iostream>
#include <filesystem>	// for obtaining the filename from the path
#include <fstream>		// for reading text files
#include <sstream>		// string-stream for splitting lines
#include <iomanip>		// for cout float decimal precision (std::fixed and std::setprecision)

#include "tinyECS/registry.hpp"
// #include "tinyECS/components.hpp"

ComponentType component_type_from_string(std::string type_string)
{
    if (type_string == "detected") return Detected;
    if (type_string == "exited") return Exited;
    if (type_string == "invader") return Invader;
    if (type_string == "moving") return Moving;
    if (type_string == "position") return Position;
    if (type_string == "queued") return Queued;
    if (type_string == "release_timer") return ReleaseTimer;
    if (type_string == "tower") return Tower;

    return Invalid;
}

//
// load_entities - load Entities, Components, and component values from filename into entity_registry
//               - returns true if successful
bool load_entities(std::string& filename, Registry& entity_registry)
{
	std::filesystem::path filename_path = std::filesystem::path(filename);
	std::string just_filename = filename_path.filename().string();
	std::cout << std::fixed;
	std::cout << std::setprecision(2);

	std::cout << "========================================" << std::endl;
	std::cout << "Reading file: " << just_filename << std::endl;
	std::cout << "========================================" << std::endl;

	// TODO: load and parse entity test file
	std::ifstream entity_file(filename);

	if (entity_file.is_open())
	{
		std::string line;

		std::string action;
		std::string name;
		std::string component_name;
		float		value;

		while (!entity_file.eof())
		{
			std::getline(entity_file, line);

			// reset line parameters
			action = "";
			name = "";
			component_name = "";
			value = -99999;

			// print each line (for debugging, NOTE: remove for submission)
			// TODO: comment out this conditional before submitting assignment
			/*if (line.length() > 0) {
				std::cout << "LINE: " << line << " [" << line.length() << "]" << std::endl;
			}*/

			if (line.length() > 0 && line[0] != '#')
			{
				// split the new line into their individual fields (dirty parser)
				std::stringstream ss_line(line);
				ss_line >> action >> name >> component_name >> value;

				if (action == "BREAK")
				{
					std::cout << "---------- " << name << " ----------" << std::endl;
				}
				// TODO: handle the "actions"
				// Add new entity or entity component for existing entity
				// syntax to add new Entity: ADD <Entity-name>
				// syntax to add new Entity Component: ADD <Entity-name> <Component-name>
				else if (action == "ADD")
				{
					auto entity_exists = entity_registry.has_entity_by_name(name);
					
					if (component_name == "")
					{
						if (entity_exists)
						{
							// An entity with that name already exists.
							auto entity = entity_registry.get_entity_by_name(name);
							std::cout << "ERROR: Entity " << name << " already exists with id " << entity.id() << "." << std::endl;
							continue;
						}

						std::cout << "ADD Entity: " << name << std::endl;
						// Adding entity
						auto entity = entity_registry.create_entity();
						entity_registry.name_entity(entity, name);
						std::cout << "New entity " << name << " added with id " << entity.id() << std::endl;
						continue;
					}
					
					// Adding component
					std::cout << "ADD Component: " << component_name << " to Entity: " << name << std::endl;
					if (!entity_exists)
					{
						std::cout << "ERROR: Failed to add component " << component_name << " to " << name << ", Entity doesn't exist." << std::endl;
						continue;
					}

					auto entity = entity_registry.get_entity_by_name(name);
					auto component_type = component_type_from_string(component_name); 

					if (component_type == Invalid || component_type & Exited || component_type & Detected || component_type & Moving)
					{
						// Some components are only to be added by systems
						std::cout << "ERROR: Tried adding invalid component type " << component_name << " to the entity " << name << " (ID: " << entity.id() << ")" << std::endl;
						continue;
					}

					if (entity_registry.has_component(entity, component_type))
					{
						// Some components are only to be added by systems
						std::cout << "ERROR: Tried adding component type " << component_name << " to the entity " << name << " (ID: " << entity.id() << "), but component already exists." << std::endl;
						continue;
					}

					if ((component_type & Invader && entity_registry.has_component(entity, Tower)) || (component_type & Tower && entity_registry.has_component(entity, Invader)))
					{
						// Some components are only to be added by systems
						std::cout << "ERROR: Tried adding component type " << component_name << " to the entity " << name << " (ID: " << entity.id() << "), but it can't be both invader and tower." << std::endl;
						continue;
					}

					
					entity_registry.add_component(entity, component_type);
					std::cout << name << " with id " << entity.id() << std::endl;
					std::cout << "Successfully added component " << component_name << " to " << name << "." << std::endl;
				}
				// Remove named components for a specific entity or remove the named entity
				// syntax to remove named Entity: REMOVE <Entity-name>
				// syntax to remove named Component for named Entity: REMOVE <Entity-name> <Component-name>
				else if (action == "REMOVE")
				{
					auto entity_exists = entity_registry.has_entity_by_name(name);

					if (component_name == "")
					{
						// remove Entity
						std::cout << "REMOVE Entity: " << name << std::endl;
	
						if (!entity_exists)
						{
							std::cout << "Unknown Entity: " << name << std::endl;
							continue;
						}
						
						auto entity = entity_registry.get_entity_by_name(name);
						entity_registry.remove_entity(entity);
						continue;
					}

					// remove Component
					std::cout << "REMOVE Component: " << component_name << " FROM Entity: " << name << std::endl;

					if (!entity_exists)
					{
						std::cout << "ERROR: Failed to remove component " << component_name << " from " << name << ", Entity doesn't exist." << std::endl;
						continue;
					}
					
					auto entity = entity_registry.get_entity_by_name(name);
					std::cout << name << " with id " << entity.id() << std::endl;

					auto component_type = component_type_from_string(component_name);
					auto has_component = entity_registry.has_component(entity, component_type);

					if (!has_component)
					{
						std::cout << "ERROR: Failed to remove component " << component_name << " from " << name << "." << std::endl;
						continue;
					}

					entity_registry.remove_component(entity, component_type);
					std::cout << "Successfully removed component " << component_name << " from " << name << "." << std::endl;
				}
				// Set individual component values for the named component
				// syntax: SET <Entity-name> <Component-name> <new-value-float>
				else if (action == "SET")
				{
					char delimeter = '.';
					std::string member_name = component_name.substr(component_name.find(delimeter) + 1, component_name.length());
					component_name = component_name.substr(0, component_name.find(delimeter));
					std::cout << "SET Component: " << component_name << "." << member_name << " for Entity: " << name << " to value: " << value << std::endl;
					auto entity_exists = entity_registry.has_entity_by_name(name);

					if (!entity_exists)
					{
						std::cout << "ERROR: Unknown Entity: " << name << " when setting Component: " << component_name << "." << member_name << "." << std::endl;
						continue;
					}

					auto entity = entity_registry.get_entity_by_name(name);
					auto component_type = component_type_from_string(component_name);
					auto entity_has_component = entity_registry.has_component(entity, component_type);

					if (!entity_has_component)
					{
						// If we don't have the component, it shouldn't work, even if it's a valid component
						std::cout << "ERROR: Failed to set value for Component: " << component_name << "." << member_name << " for Entity: " << name << "." << std::endl;
						continue;
					}

					bool set_successful = entity_registry.set_component_value(entity, component_type, member_name, value);
					
					if (!set_successful)
					{
						std::cout << "ERROR: Failed to set value for Component: " << component_name << "." << member_name << " for Entity: " << name << "." << std::endl;
						continue;
					}
				
				}
				// print all of the components and their values for the named entity
				// syntax: PRINT <Entity-name>
				else if (action == "PRINT")
				{
					std::cout << "PRINT Entity: " << name << std::endl;

					if (!entity_registry.has_entity_by_name(name))
					{
						std::cout << "ERROR: Unknown Entity: " << name << "." << std::endl;
						continue;
					}
					
					auto entity = entity_registry.get_entity_by_name(name);
					std::cout << "Entity: " << name << " has id: " << entity.id() << std::endl;
					auto is_detected = entity_registry.has_component(entity, Detected);
					auto has_exited = entity_registry.has_component(entity, Exited);
					auto is_invader = entity_registry.has_component(entity, Invader);
					auto is_moving = entity_registry.has_component(entity, Moving);
					auto has_position = entity_registry.has_component(entity, Position);
					auto is_queued = entity_registry.has_component(entity, Queued);
					auto has_release_timer = entity_registry.has_component(entity, ReleaseTimer);
					auto is_tower = entity_registry.has_component(entity, Tower);
					
					std::cout << (is_detected ? "+ " : "- ") << name << (is_detected ? " IS " : " IS NOT ") << "detected" << std::endl;
					std::cout << (has_exited ? "+ " : "- ") << name << (has_exited ? " HAS " : " HAS NOT ") << "exited" << std::endl;
					std::cout << (is_invader ? "+ " : "- ") << name << (is_invader ? " IS an " : " IS NOT an ") << "invader" << std::endl;
					std::cout << (is_moving ? "+ " : "- ") << name << (is_moving ? " IS " : " IS NOT ") << "moving" << std::endl;
					std::cout << (has_position ? "+ " : "- ") << name << (has_position ? " HAS a " : " DOES NOT have a ") << "position";

					if (has_position)
					{
						auto position = entity_registry.positions.get(entity);
						std::cout << ", with values (" << position.x << ", " << position.y << ")" << std::endl;
					}
					else
					{
						std::cout << std::endl;
					}

					std::cout << (is_queued ? "+ " : "- ") << name << (is_queued ? " IS " : " IS NOT ") << "queued" << std::endl;

					std::cout << (has_release_timer ? "+ " : "- ") << name << (has_release_timer ? " HAS a " : " DOES NOT have a ") << "release_timer";
					
					if (has_release_timer)
					{
						auto release_timer = entity_registry.release_timers.get(entity);
						std::cout << ", with timer: " << release_timer.timer << std::endl;
					}
					else
					{
						std::cout << std::endl;
					}

					std::cout << (is_tower ? "+ " : "- ") << name << (is_tower ? " IS a " : " IS NOT a ") << "tower";

					if (is_tower)
					{
						auto tower = entity_registry.towers.get(entity);
						std::cout << ", with range: " << tower.range << std::endl;
					}
					else
					{
						std::cout << std::endl;
					}
				}
				// Increase or decrease component value
				// syntax: INC|DEC <Entity-name> <Component-name> <inc-dec-value-float>
				else if (action == "INC" || action == "DEC")
				{
					auto entity_exists = entity_registry.has_entity_by_name(name);

					if (!entity_exists)
					{
						std::cout << "ERROR: Unknown Entity: " << name << " when increasing/decreasing Component: " << component_name << "." << std::endl;
						continue;
					}

					auto entity = entity_registry.get_entity_by_name(name);
					char delimeter = '.';
					std::string member_name = component_name.substr(component_name.find(delimeter) + 1, component_name.length());
					component_name = component_name.substr(0, component_name.find(delimeter));
					auto component_type = component_type_from_string(component_name);

					// For now, naive way of avoiding invalid types
					if (component_type != Position && component_type != ReleaseTimer && component_type != Tower)
					{
						std::cout << "ERROR: Trying to increase/decrease value of component " << component_name << "on Entity " << name << ", which is unable to be increased/increased." << std::endl;
						continue;
					}

					float component_value = entity_registry.get_component_value(entity, component_type, member_name);
					float add_value = value;
					
					if (action == "DEC")
					{
						add_value *= -1;
					}

					float new_value = component_value + add_value;
					bool set_successful = entity_registry.set_component_value(entity, component_type, member_name, new_value);
					
					if (!set_successful)
					{
						std::cout << "ERROR: Failed to increase/decrease value for Component: " << component_name << "." << member_name << " for Entity: " << name << "." << std::endl;
						continue;
					}
					
					std::cout << (action == "INC" ? "INCREASE" : "DECREASE") << " Component: " << component_name << "." << member_name << " for Entity:" << name << " from old value: " << component_value << " by " << value << " to new value: " << new_value << std::endl;
				}
				else {
					std::cout << "ERROR: unknown action: " << action << "." << std::endl;
				}
			}
		}

		entity_file.close();
	}
	else
	{
		std::cout << "ERROR: failed to open file: " << filename << "." << std::endl;
		return false;
	}

	std::cout << "========================================" << std::endl;

	return true;
}
