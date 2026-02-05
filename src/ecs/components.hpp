#pragma once
#include "common.hpp"
#include <vector>
#include "renderer/render_constants.hpp"

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

// used for entities that are affected by a certain colour (or player who is that colour)
struct Colour {
	COLOUR_STATE colour_state;
};

struct Collision {
	Entity collider;
	Entity collided; // the other entity involved in the collision
};

// used for Entities that cause damage (e.g. sawfish, spikes)
struct Deadly
{

};

struct PistolShrimp {
	
};

struct Sawfish {

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

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
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

struct RenderRequest
{
	TEXTURE_ASSET_ID   used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID    used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
