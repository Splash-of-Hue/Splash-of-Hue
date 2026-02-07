// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include <map>
#include <tuple>

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

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without interfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if(!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// TODO SnB9A38K: Move the player controller system
	// Reset player velocity each step
	auto& player_entity = registry.players.entities[0];
	auto& player_velocity = motions_registry.get(player_entity).velocity;
	player_velocity = vec2(0);
	
	// TODO SnB9A38K: Move some of this to player controller and leave other parts for a window controller
	for (auto& entity : registry.controls.entities)
	{
		auto& control = registry.controls.get(entity);

		//// exit game w/ ESC
		if (control.type == CONTROL_TYPE::END_GAME)
		{
			close_window();
		}

		//// Resetting game
		if (control.type == CONTROL_TYPE::RESTART && control.phase == CONTROL_PHASE::END)
		{
			int w, h;
			glfwGetWindowSize(window, &w, &h);

			restart_game();
		}

		//// Movement, on push sets the corresponding bitset value to true and on release sets it to false
		if (control.type == CONTROL_TYPE::MOVE)
		{
			if (control.phase == CONTROL_PHASE::START)
			{
				player_movement += vec2(control.value_1, control.value_2);
			}
			else if (control.phase == CONTROL_PHASE::END)
			{
				player_movement -= vec2(control.value_1, control.value_2);
			}
		}

		// TODO m1EAwQf4: Remove
		if (control.type == CONTROL_TYPE::CHANGE_RECT_TEST_SIZE)
		{
			auto& size = registry.sizes.get( mouse_shape_entity);
			size.width += control.value_2;
		}

		// TODO m1EAwQf4: Remove
		if (control.type == CONTROL_TYPE::CHANGE_RECT_TEST_COLOR)
		{
			// Map numpad to increasing or decreasing the RGB value of rect on screen
			auto rgb_index = control.value_1;
			auto value_change = control.value_2;
			auto& rect_color = registry.colors.get(mouse_shape_entity);
			rect_color[rgb_index] += value_change;
		}

		// TODO m1EAwQf4: Remove
		if (control.type == CONTROL_TYPE::MOVE_RECT_TEST)
		{
			auto& pos = registry.positions.get(mouse_shape_entity);
			pos.x = control.value_1;
			pos.y = control.value_2;
		}

		// TODO m1EAwQf4: Remove
		if (control.type == CONTROL_TYPE::PLACE_RECT_TEST && control.phase == CONTROL_PHASE::START)
		{
			auto& pos = registry.positions.get(mouse_shape_entity);
			auto& size = registry.sizes.get(mouse_shape_entity);
			auto& color = registry.colors.get(mouse_shape_entity);
			createRectangle(vec2(pos.x, pos.y), size.width, size.height, color);
		}

		if (control.type == CONTROL_TYPE::DEBUG && control.phase == CONTROL_PHASE::END)
		{
			// Toggle debug mode
			debugging.in_debug_mode = !debugging.in_debug_mode;
		}
	}

	registry.controls.clear();
	// TODO SnB9A38K: Move to player controller
	player_velocity += player_movement;

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

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());
	while (registry.renderRequests.entities.size() > 0)
		registry.remove_all_components_of(registry.renderRequests.entities.back());

	// debugging for memory/component leaks
	registry.list_all_components();

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
	{
		Entity player = Entity();

		// mark entity as player
		registry.players.emplace(player);

		// motion
		Motion& motion = registry.motions.emplace(player);
		motion.position = vec2(WINDOW_WIDTH_PX * 0.5f, WINDOW_HEIGHT_PX * 0.5f);
		motion.scale = vec2(20.0f, 20.0f);
		motion.velocity = vec2(0.0f, 0.0f);
		motion.angle = 0.0f;

		// rendering so player is visible 
		registry.positions.insert(player, motion.position);
		registry.sizes.insert(player, Size{ motion.scale.x, motion.scale.y });
		registry.circles.emplace(player);
		registry.colors.emplace(player, vec4(0.0f, 1.0f, 0.0f, 1.0));
		registry.renderRequests.insert(
			player,
			{
				EFFECT_ASSET_ID::DEFAULT,
				GEOMETRY_BUFFER_ID::CIRCLE
			}
		);

		// Colour system 
		registry.colourStates.emplace(player);
		registry.colourStates.get(player).curr_colour_state = COLOUR_STATE::NEUTRAL;
	}
	// TODO m1EAwQf4: This is only being used to make sure that the mouse rectangle isn't being destroyed on world_system reset so maybe don't need this.
	// Recreate anything that may have been destroyed by world_system's restart_game function.
	auto center_screen = vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2);
	mouse_shape_entity = createRectangle(center_screen, center_screen + vec2(200));

	// Load level
	Entity load_request_entity;
	registry.loadRequests.insert(load_request_entity, { LOAD_TYPE::LOAD_LEVEL, current_level_uid });

	player_movement = vec2(0);
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {

	ComponentContainer<Collision>& collision_container = registry.collisions;
	for (uint i = 0; i < collision_container.components.size(); i++) {
		Entity& entity = collision_container.entities[i];
		Entity& other = collision_container.get(entity).other;
		//if one entity has overlay collision, then behaviour change occurs here
		if (registry.overlayCollisions.has(entity) || registry.overlayCollisions.has(other)) {
			std::cout << "Overlay collision with entities: " << entity.id() << " " << other.id() << std::endl;
		}

		//inelastic collisions are handled in physics_system
		
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}
