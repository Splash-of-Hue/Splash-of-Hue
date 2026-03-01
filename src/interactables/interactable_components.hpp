#pragma once

#include "interactables/interactable_constants.hpp"
#include <string>
#include <vector>

struct Interactable {
	INTERACTABLE_TYPE type = INTERACTABLE_TYPE::INVALID;
};

struct Box {
};	

struct ColourOrb {

};

struct MossToggle
{
	std::vector<std::string> connected_wall_ids;
};	


/** General components **/

// for things that break on contact (i.e. boxes)
struct Breakable {
};

// for things that are collectable (i.e. colour orb by the player)
struct Collectable {
};	
