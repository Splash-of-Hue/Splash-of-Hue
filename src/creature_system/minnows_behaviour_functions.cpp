// internal
#include "common.hpp"
#include "creature_system/creature_behaviour_functions.hpp"
/*

MINNOW-SPECIFIC BEHAVIOUR FUNCTIONS WILL GO HERE
https://medium.com/better-programming/mastering-flock-simulation-with-boids-c-opengl-and-imgui-5a3ddd9cb958
Trello 1vUDbywx

*/

// Sophia note:
/*
	It might make more sense for just the leader minnow to have the creature component
	and then for all of the follower minnows to have a "Minnows" component which points
	to the leader minnow entity

	this way we don't have a bajillion creature components for all of the minnows
*/
