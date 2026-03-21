#include "interactable_system.hpp"
#include "emissions/emission_constants.hpp"
#include "shapes/shape_build_init.hpp"
#include "colour_system.hpp"
#include <ecs/registry.hpp>
#include <iostream>

void InteractableSystem::step(float elapsed_ms)
{   
    if (registry.players.entities.empty()) return;
    Entity player = registry.players.entities[0];
    std::vector<Entity> orbs_to_remove(registry.orbCollected.entities.begin(), registry.orbCollected.entities.end());

    for (Entity orb : orbs_to_remove)
    {
        if (!registry.colourOrbs.has(orb)) {
            continue;
        }

        Player& playerComp = registry.players.get(player);
        ColourOrb& orbComp = registry.colourOrbs.get(orb);

        if (orbComp.colour == COLOUR_STATE::BLUE)
        {
            playerComp.blue_unlocked = true;
            std::cout << "Player unlocked BLUE" << std::endl;
        }
        else if (orbComp.colour == COLOUR_STATE::RED)
        {
            playerComp.red_unlocked = true;
            std::cout << "Player unlocked RED" << std::endl;
        }

        registry.remove_all_components_of(orb);
    }

    for (Entity& mushroom_entity : registry.mushrooms.entities)
    {
        //only mushrooms in a radius need to undergo any changes
        if (registry.inRadius.has(mushroom_entity))
            handle_mushrooms(mushroom_entity);
        else
        {
            ColourState& mushroom_colour_state = registry.colourStates.get(mushroom_entity);
            mushroom_colour_state.prev_colour_state = mushroom_colour_state.curr_colour_state;
            mushroom_colour_state.curr_colour_state = COLOUR_STATE::NEUTRAL;
        }
    }
}

void InteractableSystem::handle_mushrooms(Entity& mushroom_entity)
{
    InRadius& in_radius_component = registry.inRadius.get(mushroom_entity);
    ColourState& mushroom_colour_state = registry.colourStates.get(mushroom_entity);

    COLOUR_STATE colour_state = change_colour_given_priority({
        {in_radius_component.inCreatureRadius, in_radius_component.creatureRadiusList},
        {in_radius_component.inPlayerRadius, in_radius_component.playerRadiusList},
        {in_radius_component.inInteractableRadius, in_radius_component.interactableRadiusList}
     });

    if (mushroom_colour_state.curr_colour_state != colour_state)
    {
        mushroom_colour_state.prev_colour_state = mushroom_colour_state.curr_colour_state;
        mushroom_colour_state.curr_colour_state = colour_state;
    }

    if (!registry.lightSources.has(mushroom_entity))
    {
        vec2 mushroom_position = registry.positions.get(mushroom_entity);
        Entity& emission_entity = createCircle(mushroom_position, 1.f, RENDER_LAYER::FLOOR_INTERACTABLES, DEFAULT_COLOUR_VEC4);
        LightEmission& light_emission_component = registry.lightEmissions.emplace(emission_entity);

        switch (registry.mushrooms.get(mushroom_entity).size)
        {
            case MUSHROOM_SIZE::SMALL:
                light_emission_component.MAX_RADIUS = EMISSION_SIZE::MUSHROOM_SMALL_EMISSION;
                break;
            case MUSHROOM_SIZE::MEDIUM:
                light_emission_component.MAX_RADIUS = EMISSION_SIZE::MUSHROOM_MEDIUM_EMISSION;
                break;
            case MUSHROOM_SIZE::LARGE:
                light_emission_component.MAX_RADIUS = EMISSION_SIZE::MUSHROOM_LARGE_EMISSION;
                break;
        }

        light_emission_component.TIME_TO_GROW = TIME_TO_GROW;
        light_emission_component.offset = EMISSION_OFFSET.at("MUSHROOM");
        PolygonMesh& emission_hitbox = registry.hitboxes.emplace(emission_entity);
        emission_hitbox = createCircleHitbox(mushroom_position, light_emission_component.MAX_RADIUS * 0.5, 8);

        //double-linking the mushroom and the emission
        light_emission_component.lightSourceEntity = mushroom_entity;
        LightSource& light_source_component = registry.lightSources.emplace(mushroom_entity);
        light_source_component.lightEmissionEntity = emission_entity;
        //assign the emission to a level
        LevelEntity& level_entity_mushroom_component = registry.levelEntities.get(mushroom_entity);
        LevelEntity& level_entity_emission_component = registry.levelEntities.emplace(emission_entity);
        level_entity_emission_component.level_uid = level_entity_mushroom_component.level_uid;
    }
    //Todo: when the mushroom assets are created, uncomment this line https://trello.com/c/sAosXtPo/98-1-animation-mushroom
    //setEntityVisualColour(mushroom_entity, colour_state);
}

//Returns the colour state of the highest-priority radius type (Player, Creature, Interactable)
COLOUR_STATE InteractableSystem::change_colour_given_priority(std::multimap<bool, std::vector<Entity>> radiiList)
{
    for (std::pair<bool, std::vector<Entity>> radiusList : radiiList)
    {
        if (radiusList.first && has_valid_emission(radiusList.second))
        {
            return registry.colourStates.get(radiusList.second[0]).curr_colour_state;
        }
    }

    return COLOUR_STATE::NEUTRAL;
}

bool InteractableSystem::has_valid_emission(std::vector<Entity>& radiusList)
{
    for (Entity& emission_entity : radiusList)
    {
        if (registry.colourStates.has(emission_entity))
            return true;
    }

    return false;
}
