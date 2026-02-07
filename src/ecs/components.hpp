#pragma once
#include "common.hpp"

// Player component
struct Player
{
	float speed = PLAYER_SPEED;
};

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

// used for Entities that cause damage
struct Deadly
{

};

// used for edible entities
struct Eatable
{

};


// Stucture to store collision information
//no longer in use
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

// Used for level entity
struct Level
{
	// Unique ID for the room
	int uid;
};

struct Creature
{

};

// TODO: Remove as we probably won't need this, this is just for M1 prototyping
struct Toggle
{

};
