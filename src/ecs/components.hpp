#pragma once
#include "common.hpp"
#include <map>

enum class COLOUR_STATE {
	NEUTRAL = 0,
	BLUE = 1,
	RED = 2
};

struct ColourPalette {
	std::map<COLOUR_STATE, vec4> state_to_colour = {
		{ COLOUR_STATE::NEUTRAL, vec4(0.5f, 0.5f, 0.5f, 1.0f)},
		{ COLOUR_STATE::BLUE, vec4(0.0f, 0.0f, 1.0f, 1.0f)},
		{ COLOUR_STATE::RED, vec4(1.0f, 0.0f, 0.0f, 1.0f)}
	};
};

struct ColourState {
	COLOUR_STATE curr_colour_state = COLOUR_STATE::NEUTRAL;
	COLOUR_STATE prev_colour_state = COLOUR_STATE::NEUTRAL;
};

struct ColourWheel {
	COLOUR_STATE prev_colour = COLOUR_STATE::NEUTRAL;
	COLOUR_STATE curr_colour = COLOUR_STATE::NEUTRAL;
};

struct ColourMenuState {
	bool is_curr_open = false;
	bool just_closed = false;
};

struct ColourMenuInput {
	int direction = 0;
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


// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

//assigned to entities that need behaviour change
struct ChangeBehaviour
{

};
//entity is assigned a type of collision (when created) to determine how its collision is resolved
struct Collidable {
	bool overlay = false;
	bool inelastic = false;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the state of the screen
struct ScreenState
{
	vec4 background_color;
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
