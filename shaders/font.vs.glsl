#version 330 core

layout(location = 0) in vec4 vertex; // vec4.xyzw = packed vec2 pos + vec2 tex_coord
out vec2 TexCoords;

uniform mat4 projection;
uniform mat3 transform;

void main()
{
	gl_Position = projection * mat4(transform) * vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
}
