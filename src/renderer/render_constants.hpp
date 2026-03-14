#pragma once
#include "common.hpp"
#include <map>
#include <string>
#include <vector>

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID
{
	PLAYER_TEXTURE_01 = 0,
	CREATURES = PLAYER_TEXTURE_01 + 1,
	SAWFISH = CREATURES + 1,
	WHALE_01 = SAWFISH + 1,
	PISTOL_SHRIMP_01 = WHALE_01 + 1,
	MINNOW_01 = PISTOL_SHRIMP_01 + 1,
	ANGLERFISH_01 = MINNOW_01 + 1,
	CORAL_TREE_01 = ANGLERFISH_01 + 1,
	MOSS = CORAL_TREE_01 + 1,
	ROOMS = MOSS + 1,
	CONTROLS_SHEET = ROOMS + 1,
	TEXTURE_COUNT = CONTROLS_SHEET + 1,
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
	DEFAULT = 0,
	COLOURED = DEFAULT + 1,
	TEXTURED = COLOURED + 1,
	VIGNETTE = TEXTURED + 1,
	EFFECT_COUNT = VIGNETTE + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID
{
	CIRCLE = 0,
	GRADIENT_CIRCLE = CIRCLE + 1,
	SPRITE = GRADIENT_CIRCLE + 1,
	RECTANGLE = SPRITE + 1,
	SCREEN_TRIANGLE = RECTANGLE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

enum class RENDER_LAYER
{
	BACKGROUND_BACK,
	BACKGROUND_FRONT,
	FLOOR,
	WALLS_BACK,
	/* Entities in these layers are sorted by Y-axis */
	FLOOR_DECORATIONS,
	FLOOR_INTERACTABLES,
	ROOM_INTERACTABLES,
	CREATURES,
	PLAYER,
	/* End */
	WALLS_FRONT,
	FOREGROUND,
	UI
};

struct VertexAttribute
{
	std::string name;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const void* pointer;
};

const std::map<EFFECT_ASSET_ID, std::vector<VertexAttribute>> shader_attributes =
{
	{
		EFFECT_ASSET_ID::DEFAULT,
		{
			{ "in_position", 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)0 },
			//{ "in_color", 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)sizeof(vec3) }
		}
	},
	{
		EFFECT_ASSET_ID::COLOURED,
		{
			{ "in_position", 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)0 }
		}
	},
	{
		EFFECT_ASSET_ID::TEXTURED,
		{
			{ "in_position", 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)0},
			//{ "in_texcoord", 2, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)sizeof(vec3) }
		}
	},
	{
		EFFECT_ASSET_ID::VIGNETTE,
		{
			{ "in_position", 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0 }
		}
	},
	{
		EFFECT_ASSET_ID::FONT,
		{
			{ "vertex", 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0 }
		}
	}
};

const std::map<EFFECT_ASSET_ID, std::vector<std::string>> shader_uniforms =
{
	{ EFFECT_ASSET_ID::DEFAULT, { "fcolor", "transform", "projection" } },
	{ EFFECT_ASSET_ID::COLOURED, { "color", "transform", "projection" } },
	{ EFFECT_ASSET_ID::TEXTURED, { "fcolor", "source_rect", "transform", "projection"  } },
	{ EFFECT_ASSET_ID::VIGNETTE, { "darken_screen_factor", "time" } },
	{ EFFECT_ASSET_ID::FONT, { "textColor", "transform", "projection" } },
};
