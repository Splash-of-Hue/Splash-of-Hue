#pragma once

#include "common.hpp"
#include "controls/control_components.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"

class ColourSystem : public SystemInterface
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
	Entity players_light_emission;
	bool colour_menu_open = false;

	// helper methods (existing)
	void openColourMenu();
	void closeColourMenu();
	void setEntityVisualColour(Entity entity, COLOUR_STATE colour_state);
	void ColourSystem::changeLightEmissions(Entity& player, int player_colour_state, int player_colour_state_prev);
	void ColourSystem::updateEmissions(float elapsed_ms);
};
