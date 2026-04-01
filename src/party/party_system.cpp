#include "ai/ai_components.hpp"
#include "algorithm"
#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "party_components.hpp"
#include "party_constants.hpp"
#include "party_system.hpp"
#include <vector>

void PartySystem::step(float elapsed_ms)
{
	for (PartyChangeRequest& party_change_request : registry.party_change_requests.components)
	{
		if (party_change_request.type == PARTY_CHANGE_REQUEST_TYPE::JOIN)
		{
			Party& joining_party = registry.parties.get(party_change_request.party);
			// Update trail to include the new member and then bump the index of the leader and other members
			joining_party.trail.insert(joining_party.trail.begin(), { registry.positions.get(party_change_request.member) });
			PartyMember& leader_member = registry.party_members.get(joining_party.leader);
			leader_member.trail_index++;

			for (Entity& party_member_entity : joining_party.other_members)
			{
				PartyMember& party_member = registry.party_members.get(party_member_entity);
				party_member.trail_index++;
			}

			// Now that indexes are updated, add new member to party and make a new party member reference
			joining_party.other_members.push_back(party_change_request.member);
			registry.party_members.insert(party_change_request.member, { party_change_request.party });
			registry.pathfinders.get(party_change_request.member).target = joining_party.trail[0];

			continue;
		}

		if (party_change_request.type == PARTY_CHANGE_REQUEST_TYPE::LEAVE)
		{
			if (!registry.party_members.has(party_change_request.member))
			{
				// Not party of any team
				continue;
			}

			PartyMember& party_member = registry.party_members.get(party_change_request.member);
			std::vector<Entity>& party_members = registry.parties.get(party_member.party).other_members;
			const auto result = std::find_if(party_members.begin(), party_members.end(), [&](Entity& e)
			{
				return e == party_change_request.member;
			});

			if (result == party_members.end())
			{
				// Wrong party to leave or is leader
				continue;
			}

			Party& party = registry.parties.get(party_member.party);
			party_members.erase(result);
			registry.party_members.remove(party_change_request.member);

			if (party_members.size() == 0)
			{
				party.trail = { registry.positions.get(party.leader) };
				registry.party_members.get(party.leader).trail_index = 0;
			}

			continue;
		}
	}

	registry.party_change_requests.clear();

	for (Party& party : registry.parties.components)
	{
		// Make a copy, not a reference, as we might store it
		const vec2 leader_position = registry.positions.get(party.leader);

		// If there are no party members, or the trail doesn't exist, keep it as just the player's location
		if (party.other_members.size() < 1 || party.trail.size() < 1)
		{
			// Keep the trail updated to the leader's position
			party.trail = { leader_position };

			continue;
		}

		// Get the end, guaranteed to contain at least one now
		// and get the element before, which is the latest party leader spot
		const vec2 latest_trail_position = *(party.trail.end() - 1);
		// If the party leader is PARTY_TRAIL_DISTANCE_STEP distance away from the latest previous leader spot, add a new spot
		const float leader_distance = distance(latest_trail_position, leader_position);

		if (leader_distance > PARTY_TRAIL_DISTANCE_STEP)
		{
			party.trail.push_back(leader_position);
			registry.party_members.get(party.leader).trail_index = party.trail.size() - 1;
		}

		// set the leader to be the current followed target which we will replace, so not by reference
		Entity current_target = party.leader;

		for (const Entity& party_member_entity : party.other_members)
		{
			// measure the distance (how many party trail spots) away from the current followed target
			PartyMember& party_member = registry.party_members.get(party_member_entity);
			PartyMember& followed_party_member = registry.party_members.get(current_target);
			const int follow_trail_distance = followed_party_member.trail_index - party_member.trail_index;
			// Get the pathfinder, since we might update it if we jump ahead
			Pathfinder& party_member_pathfinder = registry.pathfinders.get(party_member_entity);

			// if the distance is too big, set position to the index behind the followed target
			if (follow_trail_distance > PARTY_TRAIL_MAX_INDEX_DISTANCE)
			{
				const int jumping_to_index = followed_party_member.trail_index - 1;
				const vec2 new_member_position = party.trail.at(jumping_to_index);
				registry.positions.get(party_member_entity) = new_member_position;
				party_member.trail_index = jumping_to_index;
				party_member_pathfinder.target = party.trail[party_member.trail_index];

				continue;
			}

			// Check if the party member's path target has been reached, if so, update the index
			vec2& party_member_position = registry.positions.get(party_member_entity);

			if (distance(party_member_pathfinder.target, party_member_position) < party_member_pathfinder.minimum_distance_to_target + PARTY_TRAIL_DELTA)
			{
				// Only go up as far as the trail spot behind the target ahead
				party_member.trail_index = min(followed_party_member.trail_index - 1, party_member.trail_index + 1);
				party_member_pathfinder.target = party.trail[party_member.trail_index];
			}

			// Set for the next party member to follow the current one
			current_target = party_member_entity;
		}

		// If the last party member's trail index is greater than the end of the trail
		// and we have more spots on the trail than members in the party
		// reduce all member indices by 1 and remove the beginning of the trail
		const Entity& last_party_member_entity = *(party.other_members.end() - 1);
		const PartyMember& last_party_member = registry.party_members.get(last_party_member_entity);

		if (last_party_member.trail_index > 0 && party.trail.size() > party.other_members.size() + 1)
		{
			party.trail.erase(party.trail.begin());
			PartyMember& leader_member = registry.party_members.get(party.leader);
			leader_member.trail_index = max(0, leader_member.trail_index - 1);

			for (const Entity& party_member_entity : party.other_members)
			{
				PartyMember& member = registry.party_members.get(party_member_entity);
				member.trail_index = max(0, member.trail_index - 1);
			}
		}
	}
}
