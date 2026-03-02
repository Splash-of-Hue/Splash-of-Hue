#pragma once
#include "renderer/render_constants.hpp"
#include <cstdint>
#include <ecs/components.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>

struct Textured
{
	TEXTURE_ASSET_ID   used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
};

struct RenderRequest
{
	EFFECT_ASSET_ID    used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
	/// <summary>
	/// The layer to render on.
	/// </summary>
	RENDER_LAYER layer = RENDER_LAYER::FLOOR;
};

/// <summary>
/// The source of the rendered texture to pull from.
/// Useful for rendering from sprite sheets.
/// <para> [NOTE] The attributes coord and size both range from 0 to 1 as percentages. </para>
/// </summary>
struct RenderSourceRect
{
	/// <summary>
	/// The starting coordinate on the source (i.e. texture) to render. Ranges from 0 to 1.
	/// </summary>
	vec2 coord = vec2(0);
	/// <summary>
	/// The percentage size of the source to render. Ranges from 0 to 1.
	/// </summary>
	vec2 size = vec2(1);
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};
