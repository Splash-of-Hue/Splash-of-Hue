#pragma once

#include "ecs/entity.hpp"
#include "ecs/components.hpp"
#include "common.hpp"
#include "controls/control_components.hpp"

class ColourSystem
{
public:
	ColourSystem();

	void initialize_palette();

	void pollMenuState();

	void pollColourMenuInput(Control control);

	void applyColorToPlayer(COLOUR_STATE colour);

	void step(float elapsed_ms);

	// existing API kept for other use
	COLOUR_STATE getEntityColour(Entity entity);
	static void applyColour(Entity entity, COLOUR_STATE colour);
	static vec4 getPaletteColour(COLOUR_STATE state);

private:
	Entity colour_menu_entity;
	Entity colour_palette_entity;
	bool colour_menu_open = false;

	// helper methods (existing)
	void openColourMenu();
	void closeColourMenu();
	void setEntityVisualColour(Entity entity, COLOUR_STATE colour_state);
	void changeEmissionColour();
};
