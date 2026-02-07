#pragma once
#include "common.hpp"
#include <bitset>

// Player component
struct Player
{

};

// SPLASH OF HUE COMPONENTS //////////////////////////////
enum class COLOUR_STATE {
	NEUTRAL = 0,
	BLUE = 1,
	RED = 2
};

struct ColourPalette {
	std::map<COLOUR_STATE, vec3> state_to_colour = {
		{ COLOUR_STATE::NEUTRAL, vec3(0.5f, 0.5f, 0.5f)},
		{ COLOUR_STATE::BLUE, vec3(0.0f, 0.0f, 1.0f)},
		{ COLOUR_STATE::RED, vec3(1.0f, 0.0f, 0.0f)}
	};
};

// used for entities that are affected by a certain colour (or player who is that colour)
struct Colour {
	COLOUR_STATE colour_state = COLOUR_STATE::NEUTRAL;
	COLOUR_STATE prev_colour_state = COLOUR_STATE::NEUTRAL;
};

struct ColourWheel {
	COLOUR_STATE prev_colour = COLOUR_STATE::NEUTRAL;
	COLOUR_STATE curr_colour = COLOUR_STATE::NEUTRAL;
};

struct Deadly
{

};


//////////////////////////////////////////////////////////
// Tower
struct Tower {
	float range;	// for vision / detection
	float timer_ms;	// when to shoot - this could also be a separate timer component...
};

// Invader
struct Invader {
	int health;
};

// Projectile
struct Projectile {
	int damage;
};



// used for edible entities
struct Eatable
{

};

// All data relevant to the shape and motion of entities
struct Motion {
	// DEPRECATION: This will be removed in favour of the Position component
	vec2  position = { 0, 0 };
	// DEPRECATION: This will be removed in favour of the Angle component
	float angle = 0;
	vec2  velocity = { 0, 0 };
	// DEPRECATION: This will be removed in favour of the Scale component
	vec2  scale = { 10, 10 };
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

//entity is assigned a type of collision (when created) to determine how its collision is resolved
struct OverlayCollision {

};

struct InelasticCollision {

};


// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// used to hold grid line start and end positions
struct GridLine {
	vec2 start_pos = { 0,  0 };
	vec2 end_pos = { 10, 10 };	// default to diagonal line
};

// A timer that will be associated to dying chicken
struct DeathTimer
{
	float counter_ms = 3000;
};

// TODO 1ch6Q0BE: Remove in favour of robust rendering engine
// An entity that is drawn as a rectangle on screen
struct Rect {};
struct Circle {};

struct Angle
{
	float degrees;
};

struct Size
{
	float width;
	float height;
};

enum MoveKey {
	W = 0, 
	A = 1, 
	S = 2, 
	D = 3 
};

struct MoveKeys
{
	std::bitset<4> movement_keys;

	MoveKeys()
	{
		movement_keys.set(W, false);
		movement_keys.set(A, false);
		movement_keys.set(S, false);
		movement_keys.set(D, false);
	}
};
