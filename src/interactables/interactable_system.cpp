#include "animation/animation_components.hpp"
#include "animation/animation_constants.hpp"
#include "colour_system.hpp"
#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"
#include "emissions/emission_constants.hpp"
#include "interactable_components.hpp"
#include "interactable_system.hpp"
#include "shapes/shape_build_init.hpp"
#include <algorithm>
#include <ecs/registry.hpp>
#include <iostream>
#include <map>
#include <string>

void InteractableSystem::step(float elapsed_ms)
{
	if (registry.players.entities.empty()) return;

	step_moss_toggle();
	step_pressure_plates();
	step_colour_orbs();
	step_mushrooms();
}

void InteractableSystem::step_moss_toggle()
{
	for (Entity& moss_toggle_entity : registry.mossToggles.entities)
	{
		COLOUR_STATE colour_state = COLOUR_STATE::NEUTRAL;
		ColourState& moss_colour_state = registry.colourStates.get(moss_toggle_entity);

		// If the moss isn't in any colour state, set it to neutral and continue
		if (registry.inRadius.has(moss_toggle_entity))
		{
			InRadius& in_radius = registry.inRadius.get(moss_toggle_entity);
			colour_state = change_colour_given_priority(in_radius.inCreatureRadius, in_radius.creatureRadiusList,
														in_radius.inPlayerRadius, in_radius.playerRadiusList,
														in_radius.inInteractableRadius, in_radius.interactableRadiusList);
		}


		// If we are already in the needed colour state, continue
		if (moss_colour_state.curr_colour_state == colour_state)
		{
			continue;
		}

		moss_colour_state.curr_colour_state = colour_state;
		MossToggle& moss_toggle = registry.mossToggles.get(moss_toggle_entity);
		// Get the animation trigger to use based on the colour state
		std::map<COLOUR_STATE, ANIMATION_TRIGGER> colour_animation_trigger_map =
		{
			{ COLOUR_STATE::NEUTRAL, ANIMATION_TRIGGER::IDLE },
			{ COLOUR_STATE::RED, ANIMATION_TRIGGER::IDLE_RED },
			{ COLOUR_STATE::BLUE, ANIMATION_TRIGGER::IDLE_BLUE },
		};
		AnimationTrigger trigger = { colour_animation_trigger_map[colour_state] };
		registry.animation_triggers.emplace(moss_toggle_entity, trigger);

		// Update each of the connected walls that is spawned
		for (std::string& connected_wall_id : moss_toggle.connected_wall_ids)
		{
			const auto wall_id_match = [&](const Entity& colored_wall_entity)
			{
				return registry.colouredWall.get(colored_wall_entity).id == connected_wall_id;
			};
			auto& wall_entity_iterator = std::find_if(registry.colouredWall.entities.begin(), registry.colouredWall.entities.end(), wall_id_match);

			if (wall_entity_iterator == registry.colouredWall.entities.end())
			{
				continue;
			}

			registry.colourStates.get(*wall_entity_iterator).curr_colour_state = colour_state;
			registry.animation_triggers.emplace(*wall_entity_iterator, trigger);
		}
	}
}

void InteractableSystem::step_pressure_plates()
{
	for (Entity& pressure_plate_entity : registry.pressurePlates.entities)
	{
		PressurePlate& pressure_plate_component = registry.pressurePlates.get(pressure_plate_entity);
		// Bounding Box is added at creation time so no assertion needed to check if pressure plate has it
		InBoundingBox& in_bounding_box_component = registry.inBoundingBox.get(pressure_plate_entity);


		// Don't update if there hasn't been a change in the collisions
		if (!in_bounding_box_component.updatedThisStep)
		{
			continue;
		}

		// update connected walls!
		for (std::string& connected_wall_id : pressure_plate_component.connected_wall_ids)
		{
			const auto wall_id_match = [&](const Entity& pressure_plate_entity)
				{
					return registry.pressurePlateWalls.get(pressure_plate_entity).id == connected_wall_id;
				};
			auto& wall_entity_iterator = std::find_if(registry.pressurePlateWalls.entities.begin(), registry.pressurePlateWalls.entities.end(), wall_id_match);

			if (wall_entity_iterator == registry.pressurePlateWalls.entities.end())
			{
				continue;
			}

			// if the pressure plate is being pushed, open up the associated wall
			if (in_bounding_box_component.isTouchingOtherEntity)
			{
				registry.pressurePlateWalls.get(*wall_entity_iterator).is_open = false;
				registry.collidables.get(*wall_entity_iterator).inelastic = false;

				// TODO QWNAo0jQ 
				if (registry.renderRequests.has(*wall_entity_iterator))
				{
					registry.renderRequests.remove(*wall_entity_iterator);
				}
			}
			// pressure plate is not being pushed, close the wall
			else
			{
				registry.pressurePlateWalls.get(*wall_entity_iterator).is_open = true;
				registry.collidables.get(*wall_entity_iterator).inelastic = true;

				// TODO QWNAo0jQ 
				if (!registry.renderRequests.has(*wall_entity_iterator))
				{
					registry.renderRequests.insert(
						*wall_entity_iterator,
						{
							EFFECT_ASSET_ID::DEFAULT,
							GEOMETRY_BUFFER_ID::RECTANGLE,
							RENDER_LAYER::WALLS_FRONT
						}
					);
				}
			}
		}
	}
}

void InteractableSystem::step_colour_orbs()
{
	Entity player = registry.players.entities[0];
	std::vector<Entity> orbs_to_remove(registry.orbCollected.entities.begin(), registry.orbCollected.entities.end());

	for (Entity& orb : orbs_to_remove)
	{
		if (!registry.colourOrbs.has(orb))
		{
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
}

void InteractableSystem::step_mushrooms()
{
	for (Entity& mushroom_entity : registry.mushrooms.entities)
	{
		//only mushrooms in a radius need to undergo any changes
		if (registry.inRadius.has(mushroom_entity))
		{
			handle_mushrooms(mushroom_entity);
		}
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
	COLOUR_STATE colour_state = change_colour_given_priority(in_radius_component.inCreatureRadius, in_radius_component.creatureRadiusList,
															 in_radius_component.inPlayerRadius, in_radius_component.playerRadiusList,
															 in_radius_component.inInteractableRadius, in_radius_component.interactableRadiusList);

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
		//double-linking the mushroom and the emission
		light_emission_component.lightSourceEntity = mushroom_entity;
		LightSource& light_source_component = registry.lightSources.emplace(mushroom_entity);
		light_source_component.lightEmissionEntity = emission_entity;

	}
	//Todo: when the mushroom assets are created, uncomment this line https://trello.com/c/sAosXtPo/98-1-animation-mushroom
	//setEntityVisualColour(mushroom_entity, colour_state);
}

//Returns the colour state of the highest-priority radius type (Player, Creature, Interactable)
COLOUR_STATE InteractableSystem::change_colour_given_priority(bool& inRadius1, std::vector<Entity>& radiusList1, bool& inRadius2, std::vector<Entity>& radiusList2, bool& inRadius3, std::vector<Entity>& radiusList3)
{
	if (inRadius1)
		return registry.colourStates.get(radiusList1[0]).curr_colour_state;
	else if (inRadius2)
		return registry.colourStates.get(radiusList2[0]).curr_colour_state;
	else if (inRadius3)
		return registry.colourStates.get(radiusList3[0]).curr_colour_state;
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
