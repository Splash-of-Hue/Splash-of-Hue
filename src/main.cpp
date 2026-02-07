#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stdlib
#include <chrono>
#include <iostream>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "renderer/render_system.hpp"
#include "world_system.hpp"
#include "input_system.hpp"
#include "animation/animation_system.hpp"
#include "loading/loader_system.hpp"
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
	AnimationSystem animation_system;
	InputSystem input_system;
	LoaderSystem loader_system;
	CreatureBehaviourSystem creature_behaviour_system;

	// initialize window
	GLFWwindow* window = world_system.create_window();
	if (!window) {
		// Time to read the error message
		std::cerr << "ERROR: Failed to create window.  Press any key to exit" << std::endl;
		getchar();
		return EXIT_FAILURE;
	}

	if (!world_system.start_and_load_sounds()) {
		std::cerr << "ERROR: Failed to start or load sounds." << std::endl;
	}

	input_system.init(window, &world_system);

	// initialize the main systems
	renderer_system.init(window);
	world_system.init(&renderer_system);
	animation_system.init();
	input_system.on_world_reset(); // TODO m1EAwQf4: This is only being used to make sure that the mouse rectangle isn't being destroyed on world_system reset so maybe don't need this.

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
		loader_system.step(elapsed_ms);
		world_system.step(elapsed_ms);
		ai_system.step(elapsed_ms);
		creature_behaviour_system.step(elapsed_ms);
		physics_system.step(elapsed_ms);
		animation_system.step(elapsed_ms);
		world_system.handle_collisions(); // Question from Sophia: should this not come right after physics_system.step? I would imagine we'd want collisions to be handled before animations?

		renderer_system.draw();
	}

	return EXIT_SUCCESS;
}
