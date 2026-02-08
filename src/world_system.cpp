// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>

#include "physics_system.hpp"
#include "loading/loading_constants.hpp"

// create the world
WorldSystem::WorldSystem() :
	points(0),
	max_towers(MAX_TOWERS_START),
	next_invader_spawn(0),
	invader_spawn_rate_ms(INVADER_SPAWN_RATE_MS)
{
	// seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		std::cerr << error << ": " << desc << std::endl;
	}
}

// call to close the window, wrapper around GLFW commands
void WorldSystem::close_window() {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {

	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		std::cerr << "ERROR: Failed to initialize GLFW in world_system.cpp" << std::endl;
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// CK: setting GLFW_SCALE_TO_MONITOR to true will rescale window but then you must handle different scalings
	// glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);		// GLFW 3.3+
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_FALSE);		// GLFW 3.3+

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX, "Towers vs Invaders Assignment", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "ERROR: Failed to glfwCreateWindow in world_system.cpp" << std::endl;
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {

	this->renderer = renderer_arg;


	// Set all states to default
	restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Splash of Hue";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	for (auto& entity : registry.controls.entities)
	{
		Control& control = registry.controls.get(entity);

		// The rest only matter at the phase end
		if (control.phase != CONTROL_PHASE::END)
		{
			continue;
		}

		if (control.type == CONTROL_TYPE::END_GAME)
		{
			close_window();
		}

		//// Resetting game
		if (control.type == CONTROL_TYPE::RESTART)
		{
			int w, h;
			glfwGetWindowSize(window, &w, &h);

			restart_game();
		}

		if (control.type == CONTROL_TYPE::DEBUG)
		{
			// Toggle debug mode
			debugging.in_debug_mode = !debugging.in_debug_mode;
		}
	}

	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {

	std::cout << "Restarting..." << std::endl;

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Reset the game speed
	current_speed = 1.f;

	// debugging for memory/component leaks
	registry.list_all_components();
	
	while (registry.velocities.entities.size() > 0)
		registry.remove_all_components_of(registry.velocities.entities.back());
	while (registry.renderRequests.entities.size() > 0)
		registry.remove_all_components_of(registry.renderRequests.entities.back());

	int grid_line_width = GRID_LINE_WIDTH_PX;

	// create grid lines if they do not already exist
	if (grid_lines.size() == 0) {
		// vertical lines
		int cell_width = GRID_CELL_WIDTH_PX;
		for (int col = 0; col < 14 + 1; col++) {
			// width of 2 to make the grid easier to see
			grid_lines.push_back(createGridLine(vec2(col * cell_width, 0), vec2(grid_line_width, 2 * WINDOW_HEIGHT_PX)));
		}

		// horizontal lines
		int cell_height = GRID_CELL_HEIGHT_PX;
		for (int col = 0; col < 10 + 1; col++) {
			// width of 2 to make the grid easier to see
			grid_lines.push_back(createGridLine(vec2(0, col * cell_height), vec2(2 * WINDOW_WIDTH_PX, grid_line_width)));
		}
	}

	// Request a player be made
	Entity player_create_request_entity;
	registry.playerCreateRequests.emplace(player_create_request_entity);
	// Load level
	Entity load_request_entity;
	registry.loadRequests.insert(load_request_entity, { LOAD_TYPE::LOAD_LEVEL, current_level_uid });
}

// When entity collides with colour radius, it needs to have its behaviour change handled here
void WorldSystem::handle_behaviour_change()
{

	ComponentContainer<ChangeBehaviour>& changes = registry.change_behaviours;
	for (uint i = 0; i < changes.components.size(); i++)
	{
		Entity& entity = changes.entities[i];
		std::cout << "Entity has interacted with a colour radius! Entity: " << entity.id() << std::endl;
	}
	// Since behaviour change was handled, remove the entity
	registry.change_behaviours.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}
