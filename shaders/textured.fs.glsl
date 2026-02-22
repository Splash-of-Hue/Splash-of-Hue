#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec4 fcolor;
uniform vec4 source_rect;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec2 source_coord = vec2(
		source_rect.x + texcoord.x * source_rect.p,
		source_rect.y + texcoord.y * source_rect.q
	);
	color = fcolor * texture(sampler0, vec2(source_coord.x, source_coord.y));
}
