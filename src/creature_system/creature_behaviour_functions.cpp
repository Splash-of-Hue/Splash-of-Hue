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

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::joinPlayerParty(Entity& creature, float elapsed_ms)
{
	Entity& player_entity = registry.players.entities[0];
	PartyMember& player_party_member = registry.party_members.get(player_entity);
	Party& player_party = registry.parties.get(player_party_member.party);
	registry.party_members.insert(creature, { player_party_member.party });
	player_party.other_party_members.push_back(creature);
	//when something joins the party, sort party by who is currently closest to player
	return reorganizeParty(player_party.other_party_members);
}

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::leaveParty(Entity& creature, float elapsed_ms)
{
	if (!registry.party_members.has(creature))
	{
		return BEHAVIOUR_TREE_STATUS::FAILURE;
	}

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
	PartyMember& creature_party_member = registry.party_members.get(creature);
	Party& party = registry.parties.get(creature_party_member.party);
	for (int index = 0; index < party.other_party_members.size(); index++)
	{
		Entity& party_entity = party.other_party_members[index];
		Pathfinder& pathfinder = registry.pathfinders.get(party_entity);

		if (index == 0)
			pathfinder.target = party.party_leader;
		else
			pathfinder.target = party.other_party_members[index - 1];
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

BEHAVIOUR_TREE_STATUS CreatureBehaviourFunctions::reorganizeParty(std::vector<Entity>& party)
{
	if (party.size() > 1)
	{
		std::sort(party.begin(), party.end(), compareByDistanceToPlayer);
	}
	return BEHAVIOUR_TREE_STATUS::SUCCESS;
}

bool CreatureBehaviourFunctions::compareByDistanceToPlayer(const Entity& e1, const Entity& e2)
{
	vec2 player_position = registry.positions.get(registry.players.entities[0]);
	vec2 entity_1_position = registry.positions.get(e1);
	vec2 entity_2_position = registry.positions.get(e2);

	float dx1 = entity_1_position.x - player_position.x;
	float dy1 = entity_1_position.y - player_position.y;
	float dx2 = entity_2_position.x - player_position.x;
	float dy2 = entity_2_position.y - player_position.y;

	return dx1 * dx1 + dy1 * dy1 < dx2 * dx2 + dy2 * dy2;
}
