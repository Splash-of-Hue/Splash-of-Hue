#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stdlib
#include <chrono>
#include <iostream>

// internal
#include "ai/ai_system.hpp"
#include "animation/animation_system.hpp"
#include "camera/camera_system.hpp"
#include "colour_system.hpp"
#include "common.hpp"
#include "controllers/player_controller_system.hpp"
#include "controls/control_input_conversion_system.hpp"
#include "creature_system/creature_behaviour_system.hpp"
#include "emissions/emission_system.hpp"
#include "game_state/game_state_system.hpp"
#include "input/input_system.hpp"
#include "interactables/interactable_system.hpp"
#include "loading/loader_system.hpp"
#include "map/map_system.hpp"
#include "minimap/minimap_system.hpp"	
#include "physics_system.hpp"
#include "player/player_system.hpp"
#include "renderer/render_system.hpp"
#include "sound/sound_system.hpp"
#include "world_system.hpp"
#include "ui/ui_system.hpp"
#include "controllers/menu_controller_system.hpp"
#include "saving/save_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// global systems
	AISystem ai_system;
	AnimationSystem animation_system;
	CameraSystem camera_system;
	ColourSystem colour_system;
	ControlInputConversionSystem control_input_conversion_system;
	CreatureBehaviourSystem creature_behaviour_system;
	EmissionSystem emission_system;
	GameStateSystem game_state_system;
	InputSystem input_system;
	InteractableSystem interactable_system;
	LoaderSystem loader_system;
	MapSystem map_system;
	MenuControllerSystem menu_controller_system;
	MinimapSystem minimap_system;
	PhysicsSystem physics_system;
	PlayerSystem player_system;
	PlayerControllerSystem player_controller_system;
	RenderSystem renderer_system;
	SaveSystem save_system;
	SoundSystem sound_system;
	UiSystem ui_system;
	WorldSystem world_system;

	// initialize window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		std::cerr << "ERROR: Failed to create window.  Press any key to exit" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}

	if (!sound_system.start_and_load_sounds()) {
		std::cerr << "ERROR: Failed to start or load sounds." << std::endl;
	}

	// initialize the main systems
	sound_system.init();
	input_system.init(window);
	renderer_system.init(window);
	world_system.init(&renderer_system);
	animation_system.init();
	colour_system.init();
	game_state_system.init();
	// Register when each system is used
	game_state_system.register_system(&camera_system, { GAME_STATE::PLAYING });
	game_state_system.register_system(&input_system);
	game_state_system.register_system(&control_input_conversion_system);
	game_state_system.register_system(&player_controller_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&menu_controller_system, { GAME_STATE::STARTING_SCREEN, GAME_STATE::PLAYING, GAME_STATE::PAUSED });
	game_state_system.register_system(&player_system, { GAME_STATE::PLAYING });
	game_state_system.register_system(&save_system);
	game_state_system.register_system(&loader_system, GAME_STATE::LOADING);
	game_state_system.register_system(&colour_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&map_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&minimap_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&world_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&ai_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&creature_behaviour_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&physics_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&animation_system, { GAME_STATE::PLAYING, GAME_STATE::LOADING });
	game_state_system.register_system(&interactable_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&emission_system, GAME_STATE::PLAYING);
	game_state_system.register_system(&ui_system);

	// variable timestep loop
	auto t = Clock::now();
	while (!world_system.is_over()) {

		// processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// calculate elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		// Update and draw loop
		game_state_system.step(elapsed_ms);
		world_system.handle_behaviour_change();
		renderer_system.draw();
	}

	return EXIT_SUCCESS;
}
