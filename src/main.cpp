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
#include <numeric>

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


		std::map<std::string, float> test_times;
		auto get_test_ms = [&](std::chrono::steady_clock::time_point test_start, std::chrono::steady_clock::time_point  test_end)
		{
			return (float)(std::chrono::duration_cast<std::chrono::microseconds>(test_end - test_start)).count() / 1000;
		};

		auto test_start = Clock::now();
		auto test_end = Clock::now();
		float test_ms = get_test_ms(test_start, test_end);
		test_times.insert({ "Camera", test_ms });
		


		// Update and draw loop
		test_start = Clock::now();
		game_state_system.step(elapsed_ms);
		test_end = Clock::now();
		test_ms = get_test_ms(test_start, test_end);
		test_times.insert({ "Update", test_ms });

		world_system.handle_behaviour_change();
		test_start = Clock::now();
		renderer_system.draw();
		test_end = Clock::now();
		test_ms = get_test_ms(test_start, test_end);
		test_times.insert({ "Draw", test_ms });


		const auto result = std::accumulate(
			test_times.begin(),
			test_times.end(),
			0.0f, // initial value of the sum
			[](const float previous, const std::pair<const std::string, float>& p)
		{
			return previous + p.second;
		}
		);

		for (auto& time_pair : test_times)
		{
			std::cout << "TIME: " << time_pair.first << "System took " + std::to_string(time_pair.second) << "ms, or " << std::to_string(100.f * time_pair.second / result) << "% of time" << std::endl;
		}

	}

	return EXIT_SUCCESS;
}
