#pragma once

#include "ecs/entity.hpp"
#include "ecs/components.hpp"

class ColourSystem
{
public:
	ColourSystem();

	void on_collision(Entity collider, Entity collided);
	Entity open_wheel_ui(const vec2& top_left, const vec2& bottom_right);
	void close_wheel_ui(Entity wheel_entity);
	void cycle_wheel(Entity wheel_entity, bool forward);
	COLOUR_STATE getEntityColour(Entity entity);
	static void applyColour(Entity entity, COLOUR_STATE colour);
	static vec3 getPaletteColour(COLOUR_STATE state);


private:

};
