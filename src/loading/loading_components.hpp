#pragma once
#include "loading_constants.hpp"

// Used to load in new data dynamically instead of at start of the game
struct LoadRequest
{
	// The type of thing that needs loading. Could be a level, music, etc.
	LOAD_TYPE load_type;
	// The unique ID of whatever needs to be loaded
	int uid;
	// TODO BbJdP3lK: Re-assess if we need this, it was temporary fix for loading screen for M4
	bool uses_loading_screen = false;
};
