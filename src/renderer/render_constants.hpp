#pragma once

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
	SAWFISH_01 = PLAYER_TEXTURE_01 + 1,
	WHALE_01 = SAWFISH_01 + 1,
	PISTOL_SHRIMP_01 = WHALE_01 + 1,
	MINNOW_01 = PISTOL_SHRIMP_01 + 1,
	JAPANESE_PORCUPINE_CRAB_01 = MINNOW_01 + 1,
	ANGLERFISH_01 = JAPANESE_PORCUPINE_CRAB_01 + 1,
	CORAL_TREE_01 = ANGLERFISH_01 + 1,
	TEXTURE_COUNT = CORAL_TREE_01 + 1
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
