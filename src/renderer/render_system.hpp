#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "ecs/tiny_ecs.hpp"
#include "renderer/render_components.hpp"
#include <map>
#include <text/text_constants.hpp>

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem
{
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count>  texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector<std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths = {
		//std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::CHICKEN, mesh_path("chicken.obj"))
		// specify meshes of other assets here
	};

	// The font paths and default font size
	const std::array<std::pair<std::string, unsigned int>, font_count> font_paths = {
		{
			{ font_path("Aloja Extended.ttf"), 48 },
		},
	};

	// Make sure these paths remain in sync with the associated enumerators (see TEXTURE_ASSET_ID).
	const std::array<std::string, texture_count> texture_paths = {
		textures_path("Sheap_01.png"),
		textures_path("Creatures.png"),
		textures_path("sawfish-sheet.png"),
		textures_path("Whale.png"),
		textures_path("P_Shrimp_Sheet_resized.png"),
		textures_path("Minnow.png"),
		textures_path("Anglerfish.png"),
		textures_path("Coral Tree.png"),
		textures_path("moss-sheet.png"),
		textures_path("Rooms.png"),
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("default"),
		shader_path("coloured"),
		shader_path("textured"),
		shader_path("vignette"),
		shader_path("font")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices, GLenum usage);

	void initializeGlFonts();

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();

	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();

	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the vignette shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

	Entity get_screen_state_entity() { return screen_state_entity; }

private:
	// Internal drawing functions for each entity type
	void drawText(Entity entity, const mat4& projection);
	void drawShape(Entity entity, const mat3& projection); // TODO 1ch6Q0BE: Remove
	void drawGridLine(Entity entity, const mat3& projection);
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();
	// Window handle
	GLFWwindow* window;
	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;
	// Reference to the entity for screen state
	Entity screen_state_entity;
	// A map for each font and character
	std::map<FONT_ID, std::map<char, Character>> font_characters;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
std::vector<ColoredVertex> getCircleVertices(int triangle_count, float z_position, vec3 inner_color, vec3 outer_color);
std::vector<uint16_t> getCircleIndices(int triangle_count);
