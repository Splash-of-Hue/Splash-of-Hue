// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_system.hpp"

#include "algorithm"
#include "creature_components.hpp"
#include "party/party_components.hpp"
#include <iostream>
#include <vector>

CreatureBehaviourFunctions creatureBehaviourFunctions;

// ===== Colour Action Nodes =====
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeColour(Entity& creature, COLOUR_STATE colour_state)
{
	if (!registry.colourStates.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}
	ColourState& colour_state_component = registry.colourStates.get(creature);
	colour_state_component.prev_colour_state = colour_state_component.prev_colour_state;
	colour_state_component.curr_colour_state = colour_state;

	vec4& color = registry.colors.get(creature);
	const ColourPalette& palette = registry.colourPalettes.components[0];
	auto& color_from_state = palette.state_to_colour.at(colour_state_component.curr_colour_state);
	color = color_from_state;

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeBlue(Entity& creature, float elapsed_ms)
{
	return makeColour(creature, COLOUR_STATE::BLUE);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeRed(Entity& creature, float elapsed_ms)
{
	return makeColour(creature, COLOUR_STATE::RED);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::makeNeutral(Entity& creature, float elapsed_ms)
{
	return makeColour(creature, COLOUR_STATE::NEUTRAL);
}

//  ===== Movement Action Nodes =====
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::chargeAtPlayer(Entity& charger, float elapsed_ms)
{
	Entity& targeted_entity = registry.players.entities[0];
	vec2& charger_position = registry.positions.get(charger);
	vec2& targeted_entity_position = registry.positions.get(targeted_entity);
	vec2 velocity = { targeted_entity_position.x - charger_position.x,targeted_entity_position.y - charger_position.y };
	double mag = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	velocity *= (1.f / mag);
	vec2& charger_velocity = registry.velocities.get(charger);
	float charger_individual_speed = 1.f;
	switch (registry.creatures.get(charger).creatureType)
	{
		case CREATURETYPE::SAWFISH:
			charger_individual_speed = SAWFISH_CHARGING_SPEED;
			break;
	}
	charger_velocity = velocity * charger_individual_speed;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::stopCreatureMovement(Entity& creature, float elapsed_ms)
{
	if (!registry.velocities.has(creature))
		return BEHAVIOUR_TREE_STATUS::FAILURE;

	vec2& creature_velocity = registry.velocities.get(creature);
	creature_velocity = { 0.0f, 0.0f };

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeCollision(Entity& creature, float elapsed_ms)
{
	if (registry.creatureInelasticCollisions.has(creature))
	{
		registry.creatureInelasticCollisions.remove(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

//  ===== Miscellaneous Action Nodes =====
BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::applyCooldown(Entity& creature, float time)
{
	if (!registry.creatureBehaviourCooldowns.has(creature))
	{
		registry.creatureBehaviourCooldowns.emplace(creature);
	}
	registry.creatureBehaviourCooldowns.get(creature).cooldown_default = time;
	//resets to max if we call it before it finishes
	registry.creatureBehaviourCooldowns.get(creature).cooldown = registry.creatureBehaviourCooldowns.get(creature).cooldown_default;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::apply3sCooldown(Entity& creature, float elapsed_ms)
{
	return applyCooldown(creature, 3000.f);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::apply2sCooldown(Entity& creature, float elapsed_ms)
{
	return applyCooldown(creature, 2000.f);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::apply1sCooldown(Entity& creature, float elapsed_ms)
{
	return applyCooldown(creature, 1000.f);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::removeCooldown(Entity& creature, float elapsed_ms)
{
	if (registry.creatureBehaviourCooldowns.has(creature))
	{
		registry.creatureBehaviourCooldowns.remove(creature);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::returnSuccess(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::returnFailure(Entity& creature, float elapsed_ms)
{
	return BEHAVIOUR_TREE_STATUS::FAILURE;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::debugCout(Entity& creature, float elapsed_ms)
{
	std::cout << "I have made it to here in the tree!" << std::endl;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::isInParty(Entity& creature, float elapsed_ms)
{
	if (!registry.party_members.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::joinPlayerParty(Entity& creature, float elapsed_ms)
{
	Entity& player_entity = registry.players.entities[0];
	PartyMember& player_party_member = registry.party_members.get(player_entity);
	Party& player_party = registry.parties.get(player_party_member.party);
	player_party.other_party_members.push_back(creature);
	registry.party_members.insert(creature, { player_party_member.party });
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::leaveParty(Entity& creature, float elapsed_ms)
{
	PartyMember& party_member = registry.party_members.get(creature);
	std::vector<Entity>& party_members = registry.parties.get(party_member.party).other_party_members;
	const auto result = std::find_if(party_members.begin(), party_members.end(), [&](Entity& e)
	{
		return e == creature;
	});

	if (result == party_members.end())
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

	party_members.erase(result);
	registry.party_members.remove(creature);
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::setPartyTarget(Entity& creature, float elapsed_ms)
{
	PartyMember& party_member = registry.party_members.get(creature);
	Party& party = registry.parties.get(party_member.party);
	Pathfinder& pathfinder = registry.pathfinders.get(creature);
	
	if (party.other_party_members.size() == 1)
	{
		// This is the only party member, target the leader
		pathfinder.target = party.party_leader;
	}
	else
	{
		// Find where this party member is and follow the one in front
		const auto result = std::find_if(party.other_party_members.begin(), party.other_party_members.end(), [&](Entity& e)
		{
			return e == creature;
		});

		// If the creature isn't in the party, something failed
		if (result == party.other_party_members.end())
		{
			return BEHAVIOUR_TREE_STATUS::FAILURE;
		}

		// Go back 1 and dereference
		pathfinder.target = *(result - 1);
	}

	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::setPlayerTarget(Entity& creature, float elapsed_ms)
{
	Entity& player_entity = registry.players.entities[0];
	Pathfinder& pathfinder = registry.pathfinders.get(creature);
	pathfinder.target = player_entity;
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}
