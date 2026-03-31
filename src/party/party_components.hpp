#pragma once
#include "ecs/entity.hpp"
#include <vector>

struct Party
{
	Entity party_leader;
	std::vector<Entity> other_party_members;
};

struct PartyMember
{
	Entity party;
};
