#pragma once
#include "common.hpp"
#include "ecs/entity.hpp"
#include "party_constants.hpp"
#include <vector>

struct Party
{
	Entity leader;
	std::vector<Entity> other_members;
	// The trail left behind by the party leader that the members follow
	std::vector<vec2> trail;
};

struct PartyMember
{
	Entity party;
	// The index of the party trail the member is trying to get to
	int trail_index = 0;
};

struct PartyChangeRequest
{
	PARTY_CHANGE_REQUEST_TYPE type;
	Entity member;
	// The party to join or blank if leaving
	Entity party;
};
