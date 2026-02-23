#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stdlib
#include <chrono>
#include <iostream>

// internal
#include "ai_system.hpp"
#include "animation/animation_system.hpp"
#include "controllers/player_controller_system.hpp"
#include "controls/control_input_conversion_system.hpp"
#include "input/input_system.hpp"
#include "loading/loader_system.hpp"
#include "colour_system.hpp"
#include "physics_system.hpp"
#include "renderer/render_system.hpp"
#include "world_system.hpp"
#include "sound/sound_system.hpp"
#include "creature_behaviour_system/creature_behaviour_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// global systems
	AISystem	  ai_system;
	WorldSystem   world_system;
	RenderSystem  renderer_system;
	PhysicsSystem physics_system;
	SoundSystem   sound_system;
	AnimationSystem animation_system;
	InputSystem input_system;
	LoaderSystem loader_system;
	ControlInputConversionSystem control_input_conversion_system;
	PlayerControllerSystem player_controller_system;
	ColourSystem colour_system;
	CreatureBehaviourSystem creature_behaviour_system;

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
	colour_system.initialize_palette();
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

		// CK: be mindful of the order of your systems and rearrange this list only if necessary
		input_system.step(elapsed_ms);
		control_input_conversion_system.step(elapsed_ms);
		player_controller_system.step(elapsed_ms);
		loader_system.step(elapsed_ms);
		colour_system.step(elapsed_ms);
		world_system.step(elapsed_ms);
		ai_system.step(elapsed_ms);
		creature_behaviour_system.update(elapsed_ms);
		physics_system.step(elapsed_ms);
		animation_system.step(elapsed_ms);
		world_system.handle_behaviour_change();

		renderer_system.draw();
	}

	return EXIT_SUCCESS;
}
