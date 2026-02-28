#pragma once

#include "interactables/interactable_constants.hpp"

struct Interactable {
	INTERACTABLE_TYPE type = INTERACTABLE_TYPE::INVALID;
};

struct Box {
};	

struct ColourOrb {

};

struct Mushroom {
};	


/** General components **/

// for things that break on contact (i.e. boxes)
struct Breakable {
};

// for things that are collectable (i.e. colour orb by the player)
struct Collectable {
};	
