#pragma once
#include "common.hpp"
#include <map>
#include <unordered_set>

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

struct ColourMenuInput
{
	int direction = 0;
};

struct ColourIndicatorUI 
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

struct DeathTimer
{
	float counter_ms = 3000;
};

// TODO 1ch6Q0BE: Remove in favour of robust rendering engine
// An entity that is drawn as a rectangle on screen
struct Rect {};
struct Circle {};

struct Wall {

};

struct Angle
{
	float degrees;
};

struct Size
{
	float width;
	float height;
};

// TODO: Remove as we probably won't need this, this is just for M1 prototyping
struct Toggle
{
	std::vector<Entity> walls_to_toggle;
};

struct PolygonMesh 
{
	std::vector<vec2> vertices;
	std::vector<vec2> base_vertices; //vertices before rotation
	vec2 centre;
};

struct InRadius
{
	bool inPlayerRadius = false;
	bool inCreatureRadius = false;
	bool inInteractableRadius = false;
	std::vector<Entity> playerRadiusList;
	std::vector<Entity> creatureRadiusList;
	std::vector<Entity> interactableRadiusList;
};

// for non-radius positional-based collisions
struct InBoundingBox
{
	bool isTouchingOtherEntity = false;
	
	// Prevent entityTouchingList from being constantly updated every step, only update associated entities when there has been a change
	bool updatedThisStep = false;

	// unordered_set with entity pointers instead of entities bc Entity doesn't have a copy / move constructor so unordered_set breaks.
	// it's verrrrry late and I can't think of a better name but I will rename if anyone has any suggestions bec this feels mildly inappropriate. If this sounds fine I will remove this comment
	std::unordered_set<Entity*> entityTouchingList;
};

struct LightEmission
{
	Entity lightSourceEntity;
	float MAX_RADIUS = 100.f;
	float TIME_TO_GROW = 1000.f;
	vec2 offset;
};

struct LightSource
{
	Entity lightEmissionEntity;
};
