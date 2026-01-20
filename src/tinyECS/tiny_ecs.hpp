#pragma once

#include <assert.h>
#include <vector>
#include <map>
#include "components.hpp"
#include "entity.hpp"

// A container that stores components of type 'Component'
// TODO: You will have to change this class to be applicable to different component types
template <typename Component>
class ComponentContainer
{
private:
    // TODO: add variables to store components and to associate components to entities
    std::map<size_t, unsigned int> component_lookup_reverse_ = {};
    std::map<unsigned int, size_t> component_lookup_ = {};
    std::vector<Entity*> entities_ = {};
    std::vector<Component> components_ = {};
    int count = 0;

public:
    ComponentContainer() {};

    std::vector<Entity*> get_entities()
    {
        return entities_;
    };

    // Inserts a component c associated to entity e
    bool insert(Entity e, Component c)
    {
        // If we try to insert a component that already exists, we want to know it didn't work, return false
        if (has(e))
        {
            assert("Entity already contained in registry");
            return false;
        }

        // First, insert the component and get the index position
        entities_.push_back(&e);
        components_.push_back(c);
        size_t lookup_position = components_.size() - 1;
        // Then use the Entity as the key for the lookup table for the position of the component
        component_lookup_[e] = lookup_position;
        component_lookup_reverse_[lookup_position] = e;
        count++;
        return true;
    };

    bool update(Entity e, Component c)
    {
        // If we try to update a component that doesn't exist, return false
        if (!has(e))
        {
            assert("Entity not contained in registry");
            return false;
        }

        auto lookup_position = component_lookup_[e];
        components_[lookup_position] = c;
        return true;
    };

    // Checks if entity e has a component of type 'Component'
    bool has(Entity e)
    {
 		return component_lookup_.count(e) > 0;
    };

    // Removes the component of type 'Component' from entity e
    void remove(Entity e)
    {
        auto lookup_position_search = component_lookup_.find(e);

        // If we try to remove a component that does not exist, we are done
        if (lookup_position_search == component_lookup_.end())
        {
            assert("Entity does not contain in registry");
            return;
        }

        auto lookup_position = lookup_position_search->second;
        // Replace the removed component with the one at the end, taking note of the position of the end
        size_t end_entities_position = entities_.size() - 1;
        size_t end_component_position = components_.size() - 1;
        auto end_entity = entities_[end_entities_position];
        auto end_component = components_[end_component_position];
        //unsigned int end_entity_id = (*end_entity).id();
        entities_.pop_back();
        components_.pop_back();

        // If the thing being removed is not the last on the list OR the last remaining, swap the last to that spot
        if (lookup_position != end_component_position)
        {
            entities_[lookup_position] = end_entity;
            components_[lookup_position] = end_component;
            unsigned int end_entity_id = component_lookup_reverse_[end_component_position];
            component_lookup_reverse_[lookup_position] = end_entity_id;
            component_lookup_[end_entity_id] = lookup_position;
        }

        component_lookup_reverse_.erase(end_component_position);
        // Swap the reverse look-up first, to get the ID of the end component for the normal lookup
        // Now, use the ID of both components to swap the lookup position
        component_lookup_.erase(e);
        count--;
    };

    // Returns the component of type 'Component' associated with entity e
    Component get(Entity e)
    {
        auto lookup_position = component_lookup_[e];
        return components_.at(lookup_position);
    };

    int size()
    {
        return count;
    }
};