#include <glm/trigonometric.hpp>
#include <iostream>
#include <SDL.h>

// internal
#include "common.hpp"
#include "ecs/registry.hpp"
#include "renderer/render_system.hpp"
#include "ecs/registry.hpp"
#include "ecs/registry.hpp"
#include "renderer/render_system.hpp"
#include "renderer/render_system.hpp"
#include <stb_image.h>
#include <string>
#include <vector>

//
void RenderSystem::drawText(Entity entity, const mat4& projection)
{
	// Make a copy of the position so we can change it
	auto position = registry.positions.get(entity);
	auto& scale = registry.scales.get(entity);
	auto& text_string = registry.texts.get(entity).text;
	auto& font_face = registry.texts.get(entity).font_face;

	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(position);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum == (GLuint)EFFECT_ASSET_ID::FONT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry == GEOMETRY_BUFFER_ID::SCREEN_TEXT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "textColor");
	const vec4 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec4(1);
	// CK: std::cout << "line color: " << color.r << ", " << color.g << ", " << color.b << std::endl;
	glUniform4fv(color_uloc, 1, (float*)&color);
	gl_has_errors();


	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	gl_has_errors();

	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	GLint in_vertex_loc = glGetAttribLocation(program, "vertex");
	glEnableVertexAttribArray(in_vertex_loc);
	glVertexAttribPointer(in_vertex_loc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	gl_has_errors();

	const auto& font = font_characters[font_face];
	// iterate through characters of the text string
	std::string::const_iterator c;

	for (c = text_string.begin(); c != text_string.end(); c++)
	{
		gl_has_errors();
		Character ch = font.at(*c);
		float xpos = position.x + ch.Bearing.x * scale.x;
		float ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale.y;

		float w = ch.Size.x * scale.x;
		float h = ch.Size.y * scale.y;

		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		// bind current char texture
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		gl_has_errors();
		// std::cout << "binding texture: " << ch.character << " = " << ch.TextureID << std::endl;

		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		gl_has_errors();

		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		gl_has_errors();

		// advance position for next glyph (note that advance is number of 1/64 pixels)
		position.x += (ch.Advance >> 6) * scale.x; // bitshift by 6 to get value in pixels (2^6 = 64)
		gl_has_errors();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderSystem::load_texture(const TextureLoadRequest& texture_load_request)
{
	// Load in the image from path
	ivec2& dimensions = ivec2(0);
	stbi_uc* data;
	data = stbi_load(texture_load_request.path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

	if (data == NULL)
	{
		const std::string message = "Could not load the file " + texture_load_request.path + ".";
		fprintf(stderr, "%s", message.c_str());
		assert(false);
	}

	// Generate texture
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// Set images properties
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Check for errors and free memory
	gl_has_errors();
	stbi_image_free(data);
	gl_has_errors();
	// If it worked, keep a reference to it in a component
	Entity loaded_image_entity;
	registry.loadedImages.insert(loaded_image_entity,
								 {
									 texture_load_request.uid,
									 texture_id,
									 texture_load_request.path,
									 dimensions,
								 });

	// Reset back
	glBindTexture(GL_TEXTURE_2D, 0);
}

// TODO 1ch6Q0BE: Remove function
void RenderSystem::drawShape(Entity entity, const mat3& projection)
{

	vec2& position = registry.positions.get(entity);
	Size& size_component = registry.sizes.get(entity);

	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(vec2(position.x, position.y));

	if (registry.cameras.size() > 0 && !registry.ignoredByCamera.has(entity))
	{
		const vec2& camera_position = registry.positions.get(registry.cameras.entities[0]);
		transform.translate(window_center() - camera_position);
	}

	transform.scale(vec2(size_component.width, size_component.height));

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	if (render_request.used_effect == EFFECT_ASSET_ID::DEFAULT)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		gl_has_errors();

		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec4 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec4(1);
	glUniform4fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	gl_has_errors();

	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	vec2& position = registry.positions.get(entity);
	vec2& scale = registry.scales.get(entity);
	Angle& angle = registry.angles.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(position);

	if (registry.cameras.size() > 0 && !registry.ignoredByCamera.has(entity))
	{
		const vec2& camera_position = registry.positions.get(registry.cameras.entities[0]);
		transform.translate(window_center() - camera_position);
	}

	vec2 velocity = registry.velocities.has(entity) ? registry.velocities.get(entity) : vec2(0);
	vec2 flipped_scale = vec2(velocity.x < 0 ? -scale.x : scale.x, scale.y);
	transform.scale(flipped_scale);
	float visual_offset = registry.visualRotationOffsets.has(entity) ? registry.visualRotationOffsets.get(entity).degrees : 0.f;
	transform.rotate(radians(angle.degrees + visual_offset));

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

		for (TexturedVertex textured_vertex : textured_vertices)
		{
			textured_vertex.position = transform.mat * textured_vertex.position;
			batch_texture_vertices.push_back(textured_vertex);
		}
	}

	// Do the same as above but with the current program and texture
	if (!batch_texture_vertices.empty())
	{
		// Update the buffer for the new batch
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch_texture_vertices.size() * sizeof(TexturedVertex), batch_texture_vertices.data());
		// Update the state
		glUseProgram(current_program);
		glBindTexture(GL_TEXTURE_2D, current_texture);
		// 
		
		// Batch draw
		glDrawArrays(GL_TRIANGLES, 0, batch_texture_vertices.size());
	}	

	//GLint color_uloc = uniform_locations[program]["fcolor"];
	//const vec4 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec4(1);
	//glUniform4fv(source_rect_uloc, 1, (float*)&render_source_rect);

	//GLint source_rect_uloc = uniform_locations[program]["source_rect"];
	//const RenderSourceRect& render_source_rect = registry.renderSourceRects.has(entity) ? registry.renderSourceRects.get(entity) : RenderSourceRect();
	//glUniform4fv(source_rect_uloc, 1, (float*)&render_source_rect);



	// Setting shaders
	//glUseProgram(program);
	//gl_has_errors();

	//assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	//const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	//const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	//// Setting vertex and index buffers
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//gl_has_errors();

	//GLint in_position_loc = attribute_locations[program]["in_position"];
	//GLint in_texcoord_loc = attribute_locations[program]["in_texcoord"];
	//gl_has_errors();
	//assert(in_texcoord_loc >= 0);
	//glEnableVertexAttribArray(in_position_loc);
	//glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
	//						sizeof(TexturedVertex), (void*)0);
	//gl_has_errors();

	//glEnableVertexAttribArray(in_texcoord_loc);
	//glVertexAttribPointer(
	//	in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
	//	(void*)sizeof(
	//	vec3)); // note the stride to skip the preceeding vertex position

	//// Enabling and binding texture to slot 0
	//glActiveTexture(GL_TEXTURE0);
	//gl_has_errors();

	//assert(registry.textured.has(entity));
	//GLuint texture_id =
	//	texture_gl_handles[(GLuint)registry.textured.get(entity).used_texture];

	//glBindTexture(GL_TEXTURE_2D, texture_id);
	//gl_has_errors();
	//


	//GLint color_uloc = uniform_locations[program]["fcolor"];
	//const vec4 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec4(1);
	//glUniform4fv(color_uloc, 1, (float*)&color);

	//// Getting the size and area to drawn from the source
	//GLint source_rect_uloc = uniform_locations[program]["source_rect"];
	//const RenderSourceRect& render_source_rect = registry.renderSourceRects.has(entity) ? registry.renderSourceRects.get(entity) : RenderSourceRect();
	//glUniform4fv(source_rect_uloc, 1, (float*)&render_source_rect);

	//// Get number of indices from index buffer, which has elements uint16_t
	//GLint size = 0;
	//glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	//gl_has_errors();

	//GLsizei num_indices = size / sizeof(uint16_t);
	//// GLsizei num_triangles = num_indices / 3;

	//GLint currProgram;
	//glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	//// Setting uniform values to the currently bound program
	//GLint transform_loc = uniform_locations[program]["transform"];
	//glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	//gl_has_errors();

	//GLint projection_loc = uniform_locations[program]["projection"];
	//glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	//gl_has_errors();

	//// Drawing of num_indices/3 triangles specified in the index buffer
	//glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	//gl_has_errors();
}

// first draw to an intermediate texture,
// apply the "vignette" texture, when requested
// then draw the intermediate texture
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the vignette texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::VIGNETTE]);
	gl_has_errors();

	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(CLEAR_SCREEN_COLOR.r, CLEAR_SCREEN_COLOR.g, CLEAR_SCREEN_COLOR.b, CLEAR_SCREEN_COLOR.a);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
	// indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	// add the "vignette" effect
	const GLuint vignette_program = effects[(GLuint)EFFECT_ASSET_ID::VIGNETTE];

	// set clock
	GLuint time_uloc = glGetUniformLocation(vignette_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(vignette_program, "darken_screen_factor");

	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(vignette_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();

	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
	// no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);

	// Background colour
	ScreenState& screen = registry.screenStates.get(screen_state_entity);
	glClearColor(screen.background_color.r, screen.background_color.g, screen.background_color.b, screen.background_color.a);

	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
	// and alpha blending, one would have to sort
	// sprites back to front
	gl_has_errors();

	mat4 screen_projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH_PX), 0.0f, static_cast<float>(WINDOW_HEIGHT_PX));
	mat3 projection_2D = createProjectionMatrix();

	// We have to sort each frame in case things move around
	// If things haven't changed, it should be fast to sort
	registry.renderRequests.sort([&]
	(Entity rr_a_entity, Entity rr_b_entity)
	{
		const RENDER_LAYER& a_layer = registry.renderRequests.get(rr_a_entity).layer;
		const RENDER_LAYER& b_layer = registry.renderRequests.get(rr_b_entity).layer;

		if (a_layer < RENDER_LAYER::FLOOR_DECORATIONS || b_layer < RENDER_LAYER::FLOOR_DECORATIONS
			|| a_layer > RENDER_LAYER::PLAYER || b_layer > RENDER_LAYER::PLAYER)
		{
			return a_layer < b_layer;
		}

		// Some need to be sorted by where they are in the level, since they move around
		// This way it looks like things pass each other
		const float& a_y_bottom = registry.positions.get(rr_a_entity).y + registry.scales.get(rr_a_entity).y;
		const float& b_y_bottom = registry.positions.get(rr_b_entity).y + registry.scales.get(rr_b_entity).y;

		return a_y_bottom < b_y_bottom;
	}
	);

	registry.textured.sort([&]
	(Entity texture_a_entity, Entity texture_b_entity)
	{
		const RenderRequest& a_render_request = registry.renderRequests.get(texture_a_entity);
		const RenderRequest& b_render_request = registry.renderRequests.get(texture_b_entity);

		// Sort first by layer
		if (a_render_request.layer != b_render_request.layer)
		{
			return a_render_request.layer < a_render_request.layer;
		}

		const TEXTURE_ASSET_ID& a_texture_id = registry.textured.get(texture_a_entity).used_texture;
		const TEXTURE_ASSET_ID& b_texture_id = registry.textured.get(texture_b_entity).used_texture;

		// Then sort by texture if they share a layer
		if (a_texture_id != b_texture_id)
		{
			return a_texture_id < b_texture_id;
		}

		// If they are the same layer and same texture, check if we are using different shaders
		return a_render_request.used_effect < a_render_request.used_effect;
	}
	);


	drawTextures(projection_2D);

	// draw all entities with a render request to the frame buffer
	for (Entity& entity : registry.renderRequests.entities)
	{
		// draw grid lines separately, as they do not have motion but need to be rendered
		if (registry.gridLines.has(entity))
		{
			drawGridLine(entity, projection_2D);
		}
		else if (registry.rectangles.has(entity) || registry.circles.has(entity))
		{
			drawShape(entity, projection_2D);
		}
		else if (registry.texts.has(entity))
		{
			drawText(entity, screen_projection);
		}
	}

	// draw framebuffer to screen
	// adding "vignette" effect when applied
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();

	// Create any texture load requests
	if (registry.textureLoadRequests.size() > 0)
	{
		for (TextureLoadRequest& texture_load_request : registry.textureLoadRequests.components)
		{
			load_texture(texture_load_request);
		}

		registry.textureLoadRequests.clear();
	}
}

mat3 RenderSystem::createProjectionMatrix()
{
	// fake projection matrix, scaled to window coordinates
	float left = 0.f;
	float top = 0.f;
	float right = (float)WINDOW_WIDTH_PX;
	float bottom = (float)WINDOW_HEIGHT_PX;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);

	return
	{
		{ sx, 0.f, 0.f},
		{0.f,  sy, 0.f},
		{ tx,  ty, 1.f}
	};
}
