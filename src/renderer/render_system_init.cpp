// stdlib
#include <iostream>
#include <sstream>
#include <array>
#include <fstream>
#include <freetype/freetype.h>

// internal
#include "../ext/stb_image/stb_image.h"
#include "renderer/render_system.hpp"
#include "ecs/registry.hpp"
#include <text/text_constants.hpp>
#include "render_constants.hpp"
#include "render_components.hpp"


// Render initialization
bool RenderSystem::init(GLFWwindow* window_arg)
{
	this->window = window_arg;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	const int is_fine = gl3w_init();
	assert(is_fine == 0);

	// Create a frame buffer
	frame_buffer = 0;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// For some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int frame_buffer_width_px, frame_buffer_height_px;
	glfwGetFramebufferSize(window, &frame_buffer_width_px, &frame_buffer_height_px);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	if (frame_buffer_width_px != WINDOW_WIDTH_PX)
	{
		printf("WARNING: retina display! https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value\n");
		printf("glfwGetFramebufferSize = %d,%d\n", frame_buffer_width_px, frame_buffer_height_px);
		printf("requested window width,height = %d,%d\n", WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX);
	}

	// Hint: Ask your TA for how to setup pretty OpenGL error callbacks. 
	// This can not be done in mac os, so do not enable
	// it unless you are on Linux or Windows. You will need to change the window creation
	// code to use OpenGL 4.3 (not suported on mac) and add additional .h and .cpp
	// glDebugMessageCallback((GLDEBUGPROC)errorCallback, nullptr);

	// We are not really using VAO's but without at least one bound we will crash in
	// some systems.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	gl_has_errors();

	initScreenTexture();
    initializeGlTextures();
	initializeGlEffects();
	initializeGlGeometryBuffers();
	initializeGlFonts();

	return true;
}

void RenderSystem::initializeGlFonts()
{
	// init FreeType fonts
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	for (uint i = 0; i < font_paths.size(); i++)
	{
		const auto& font_info = font_paths[i];
		const auto& font_path = font_info.first;
		const auto& font_default_size = font_info.second;
		std::map<char, Character> character_map;

		FT_Face face;
		if (FT_New_Face(ft, font_path.c_str(), 0, &face))
		{
			std::cerr << "ERROR::FREETYPE: Failed to load font: " << font_path << std::endl;
			return;
		}

		// extract a default size
		if (FT_Set_Pixel_Sizes(face, 0, font_default_size))
		{
			std::cerr << "ERROR::FREETYPE: Failed to set font size: " << font_path << std::endl;
			return;
		}

		// disable byte-alignment restriction in OpenGL
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Ensure an unicode characater map is loaded
		FT_Select_Charmap(face, FT_ENCODING_UNICODE);

		FT_ULong charcode;
		FT_UInt gid;

		charcode = FT_Get_First_Char(face, &gid);
		while (gid != 0)
		{
			// load character glyph
			if (FT_Load_Char(face, charcode, FT_LOAD_RENDER))
			{
				std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			// std::cout << "texture: " << c << " = " << texture << std::endl;

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// now store character for later use
			Character character = {
				texture,
				ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x),
				(unsigned char)charcode
			};

			character_map.insert({ charcode, character });
			charcode = FT_Get_Next_Char(face, charcode, &gid);
		}

		font_characters.insert({ (FONT_ID)i, character_map });
		glBindTexture(GL_TEXTURE_2D, 0);
		// clean up
		FT_Done_Face(face);
	}

	FT_Done_FreeType(ft);
	gl_has_errors();
}

void RenderSystem::initializeGlTextures()
{
    glGenTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());

    for(uint i = 0; i < texture_paths.size(); i++)
    {
		const std::string& path = texture_paths[i];
		ivec2& dimensions = texture_dimensions[i];

		stbi_uc* data;
		data = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

		if (data == NULL)
		{
			const std::string message = "Could not load the file " + path + ".";
			fprintf(stderr, "%s", message.c_str());
			assert(false);
		}
		glBindTexture(GL_TEXTURE_2D, texture_gl_handles[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl_has_errors();
		stbi_image_free(data);
    }
	gl_has_errors();
}

void RenderSystem::initializeGlEffects()
{
	for(uint i = 0; i < effect_paths.size(); i++)
	{
		const std::string vertex_shader_name = effect_paths[i] + ".vs.glsl";
		const std::string fragment_shader_name = effect_paths[i] + ".fs.glsl";

		bool is_valid = loadEffectFromFile(vertex_shader_name, fragment_shader_name, effects[i]);
		assert(is_valid && (GLuint)effects[i] != 0);
		GLuint effect_id = (GLuint)effects[i];
		std::map<std::string, GLint> attribute_map;
		std::map<std::string, GLint> uniform_map;

		for (auto& shader_attribute : shader_attributes.at((EFFECT_ASSET_ID)i))
		{
			GLint attribute_location = glGetAttribLocation(effect_id, shader_attribute.name.c_str());
			attribute_map[shader_attribute.name] = attribute_location;
			glEnableVertexAttribArray(attribute_location);
			glVertexAttribPointer(attribute_location, shader_attribute.size, shader_attribute.type, shader_attribute.normalized, shader_attribute.stride, shader_attribute.pointer);
			gl_has_errors();
		}

		for (auto& shader_uniform : shader_uniforms.at((EFFECT_ASSET_ID)i))
		{
			GLint uniform_location = glGetUniformLocation(effect_id, shader_uniform.c_str());
			uniform_map[shader_uniform] = uniform_location;
		}

		attribute_locations[effect_id] = attribute_map;
		uniform_locations[effect_id] = uniform_map;
		gl_has_errors();
	}
}

// One could merge the following two functions as a template function...
template <class T>
void RenderSystem::bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices, GLenum usage)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), usage);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices[0]) * indices.size(), indices.data(), usage);
	gl_has_errors();
}

void RenderSystem::initializeGlMeshes()
{
	for (uint i = 0; i < mesh_paths.size(); i++)
	{
		// Initialize meshes
		GEOMETRY_BUFFER_ID geom_index = mesh_paths[i].first;
		std::string name = mesh_paths[i].second;
		Mesh::loadFromOBJFile(name, 
			meshes[(int)geom_index].vertices,
			meshes[(int)geom_index].vertex_indices,
			meshes[(int)geom_index].original_size);

		bindVBOandIBO(geom_index,
			meshes[(int)geom_index].vertices, 
			meshes[(int)geom_index].vertex_indices,
			GL_STATIC_DRAW);
	}
}

void RenderSystem::initializeGlGeometryBuffers()
{
	// Vertex Buffer creation.
	glGenBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	// Index Buffer creation.
	glGenBuffers((GLsizei)index_buffers.size(), index_buffers.data());

	// Index and Vertex buffer data initialization.
	initializeGlMeshes();

	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	const std::vector<vec3> vertex_positions = {
		{ -0.5f, +0.5f, 0.f },
		{ +0.5f, +0.5f, 0.f },
		{ +0.5f, -0.5f, 0.f },
		{ -0.5f, -0.5f, 0.f }
	};
	const std::vector<vec2> vertex_texcoords = {
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f }
	};

	for (int i = 0; i < 4; i++)
	{
		textured_vertices.push_back({ vertex_positions[i], vertex_texcoords[i] });
	}

	// Counterclockwise as it's the default OpenGL front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITE, textured_vertices, textured_indices, GL_STATIC_DRAW);

	////////////////////////
	// Initialize normal circle
	constexpr int CIRCLE_NUM_TRIANGLES = 31;
	constexpr float z = -0.1f;
	constexpr vec3 white_color = vec3(1);
	constexpr vec3 light_grey_color = white_color - vec3(0.2);
	std::vector<ColoredVertex> circle_vertices = getCircleVertices(CIRCLE_NUM_TRIANGLES, z, white_color, white_color);
	std::vector<uint16_t> circle_indices = getCircleIndices(CIRCLE_NUM_TRIANGLES);
	int geom_index = (int)GEOMETRY_BUFFER_ID::CIRCLE;
	meshes[geom_index].vertices = circle_vertices;
	meshes[geom_index].vertex_indices = circle_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::CIRCLE, meshes[geom_index].vertices, meshes[geom_index].vertex_indices, GL_STATIC_DRAW);

	////////////////////////
	// Initialize gradient circle
	constexpr int GRAD_CIRCLE_NUM_TRIANGLES = 62;
	std::vector<ColoredVertex> grad_circle_vertices = getCircleVertices(GRAD_CIRCLE_NUM_TRIANGLES, z, light_grey_color, white_color);
	std::vector<uint16_t> grad_circle_indices = getCircleIndices(GRAD_CIRCLE_NUM_TRIANGLES);
	geom_index = (int)GEOMETRY_BUFFER_ID::GRADIENT_CIRCLE;
	meshes[geom_index].vertices = grad_circle_vertices;
	meshes[geom_index].vertex_indices = grad_circle_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::GRADIENT_CIRCLE, meshes[geom_index].vertices, meshes[geom_index].vertex_indices, GL_STATIC_DRAW);
	//////////////////////////////////
	// Initialize debug line
	std::vector<ColoredVertex> line_vertices;
	std::vector<uint16_t> line_indices;
	constexpr float depth = 0.5f;
	// constexpr vec3 red = { 0.8, 0.1, 0.1 };
	constexpr vec3 red = { 1.0, 1.0, 1.0 };
	// Two triangles
	line_indices = { 0, 1, 3, 1, 2, 3 };
	// Corner points
	line_vertices = {
		{{-0.5,-0.5, depth}, red},
		{{-0.5, 0.5, depth}, red},
		{{ 0.5, 0.5, depth}, red},
		{{ 0.5,-0.5, depth}, red},
	};
	geom_index = (int)GEOMETRY_BUFFER_ID::RECTANGLE;
	meshes[geom_index].vertices = line_vertices;
	meshes[geom_index].vertex_indices = line_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::RECTANGLE, line_vertices, line_indices, GL_STATIC_DRAW);

	///////////////////////////////////////////////////////
	// Initialize screen triangle (yes, triangle, not quad; its more efficient).
	std::vector<vec3> screen_vertices(3);
	screen_vertices[0] = { -1, -6, 0.f };
	screen_vertices[1] = {  6, -1, 0.f };
	screen_vertices[2] = { -1,  6, 0.f };
	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> screen_indices = { 0, 1, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE, screen_vertices, screen_indices, GL_STATIC_DRAW);

	///////////////////////////////////////////////////////
	// Initialize font VBO.
	std::vector<vec4> text_vertices(6);
	const std::vector<uint16_t> text_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SCREEN_TEXT, text_vertices, text_indices, GL_DYNAMIC_DRAW);
}

RenderSystem::~RenderSystem()
{
	// Don't need to free gl resources since they last for as long as the program,
	// but it's polite to clean after yourself.
	glDeleteBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	glDeleteBuffers((GLsizei)index_buffers.size(), index_buffers.data());
	glDeleteTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());
	glDeleteTextures(1, &off_screen_render_buffer_color);
	glDeleteRenderbuffers(1, &off_screen_render_buffer_depth);
	gl_has_errors();

	for(uint i = 0; i < effect_count; i++) {
		glDeleteProgram(effects[i]);
	}
	// delete allocated resources
	glDeleteFramebuffers(1, &frame_buffer);
	gl_has_errors();

	// remove all entities created by the render system
	while (registry.renderRequests.entities.size() > 0)
	    registry.remove_all_components_of(registry.renderRequests.entities.back());
}

// Initialize the screen texture from a standard sprite
bool RenderSystem::initScreenTexture()
{
	// create a single entry
	registry.screenStates.emplace(screen_state_entity);

	int framebuffer_width, framebuffer_height;
	glfwGetFramebufferSize(const_cast<GLFWwindow*>(window), &framebuffer_width, &framebuffer_height);  // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	glGenTextures(1, &off_screen_render_buffer_color);
	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl_has_errors();

	glGenRenderbuffers(1, &off_screen_render_buffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, off_screen_render_buffer_depth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, off_screen_render_buffer_color, 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer_width, framebuffer_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, off_screen_render_buffer_depth);
	gl_has_errors();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return true;
}

std::vector<ColoredVertex> getCircleVertices(int triangle_count, float z_position, vec3 inner_color, vec3 outer_color)
{
	std::vector<ColoredVertex> vertices;

	for (int i = 0; i < triangle_count; i++)
	{
		const float t = float(i) * M_PI * 2.f / float(triangle_count - 1);
		vertices.push_back({});
		vertices.back().position = { 0.5 * cos(t), 0.5 * sin(t), z_position };
		vertices.back().color = { outer_color.r, outer_color.g, outer_color.b };
	}

	vertices.push_back({});
	vertices.back().position = { 0, 0, 0 };
	vertices.back().color = { inner_color.r, inner_color.g, inner_color.b };

	return vertices;
}

std::vector<uint16_t> getCircleIndices(int triangle_count)
{
	std::vector<uint16_t> indices;

	// Wedges towards middle
	for (int i = 0; i < triangle_count; i++)
	{
		indices.push_back((uint16_t)i);
		indices.push_back((uint16_t)((i + 1) % triangle_count));
		indices.push_back((uint16_t)triangle_count);
	}

	return indices;
}

bool gl_compile_shader(GLuint shader)
{
	glCompileShader(shader);
	gl_has_errors();
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader, log_len, &log_len, log.data());
		glDeleteShader(shader);

		gl_has_errors();

		fprintf(stderr, "GLSL: %s", log.data());
		return false;
	}

	return true;
}

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program)
{
	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);
	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path.c_str(), fs_path.c_str());
		assert(false);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);
	gl_has_errors();

	// Compiling
	if (!gl_compile_shader(vertex))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}
	if (!gl_compile_shader(fragment))
	{
		fprintf(stderr, "Fragment compilation failed");
		assert(false);
		return false;
	}

	// Linking
	out_program = glCreateProgram();
	glAttachShader(out_program, vertex);
	glAttachShader(out_program, fragment);
	glLinkProgram(out_program);
	gl_has_errors();

	{
		GLint is_linked = GL_FALSE;
		glGetProgramiv(out_program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(out_program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(out_program, log_len, &log_len, log.data());
			gl_has_errors();

			fprintf(stderr, "Link error: %s", log.data());
			assert(false);
			return false;
		}
	}

	// No need to carry this around. Keeping these objects is only useful if we recycle
	// the same shaders over and over, which we don't, so no need and this is simpler.
	glDetachShader(out_program, vertex);
	glDetachShader(out_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	gl_has_errors();

	return true;
}
