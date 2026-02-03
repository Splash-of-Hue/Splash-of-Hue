#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;
uniform float hurt_screen_factor;

in vec2 texcoord;
// 0 if not applied, 1 otherwise
//in int vignette_applied;

layout(location = 0) out vec4 color;

vec4 vignette(vec4 in_color) 
{
	// HANDLE THE VIGNETTE EFFECT HERE
	float edge_slide = length(texcoord - vec2(0.5)) / length(vec2(0.5));
	vec4 red_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	float toggle_hurt = mix(0, max(0, edge_slide - 0.5), hurt_screen_factor);
	vec4 fading_hurt_color = mix(in_color, red_color, toggle_hurt);
	
	return in_color = fading_hurt_color;
}

// darken the screen, i.e., fade to black
vec4 fade_color(vec4 in_color)
{
	// Remove if statement from shader
	vec4 dark_grey_color = vec4(0.8, 0.8, 0.8, 0);
	in_color = mix(in_color, in_color - darken_screen_factor * dark_grey_color, step(0, darken_screen_factor));

	//if (darken_screen_factor > 0)
		//in_color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);
	return in_color;
}

void main()
{
    vec4 in_color = texture(screen_texture, texcoord);
    color = fade_color(vignette(in_color));
}