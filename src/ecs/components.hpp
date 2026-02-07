#pragma once
#include "common.hpp"
#include <bitset>
#include <vector>
#include "renderer/render_constants.hpp"
#include "loading/loading_constants.hpp"

// Player component
struct Player
{

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
