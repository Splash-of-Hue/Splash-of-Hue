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
	void glfw_err_cb(int error, const char* desc) {
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

	// TODO SnB9A38K: Move the player controller system
	Entity& player_entity = registry.players.entities[0];
	vec2& player_pos = registry.positions.get(player_entity);
	vec2& player_vel = registry.velocities.get(player_entity);
	float player_speed = registry.players.get(player_entity).speed;
	player_vel = vec2(0);

	// TODO SnB9A38K: Move some of this to player controller and leave other parts for a window controller
	for (Entity& entity : registry.controls.entities)
	{
		Control& control = registry.controls.get(entity);

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
			auto& size = registry.sizes.get(mouse_shape_entity);
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
			auto& colourState = registry.colourStates.get(mouse_shape_entity);
			createRectangle(vec2(pos.x, pos.y), size.width, size.height, color, true, int(colourState.curr_colour_state));
		}

		if (control.type == CONTROL_TYPE::DEBUG && control.phase == CONTROL_PHASE::END)
		{
			// Toggle debug mode
			debugging.in_debug_mode = !debugging.in_debug_mode;
		}

		//testing coloured wall collisions, need to manually change state of rectangle
		if (control.type == CONTROL_TYPE::CHANGE_RECT_TEST_STATE && control.phase == CONTROL_PHASE::START)
		{
			ColourState& state = registry.colourStates.get(mouse_shape_entity);
			state.prev_colour_state = state.curr_colour_state;
			state.curr_colour_state = COLOUR_STATE(control.value_1);

			vec4& color = registry.colors.get(mouse_shape_entity);
			switch (state.curr_colour_state) {
			case COLOUR_STATE::NEUTRAL:
				color = vec4{ 1.f, 1.f, 1.f, 1.f };
				break;
			case COLOUR_STATE::BLUE:
				color = vec4{ 0.f, 0.f, 1.f, 1.f };
				break;
			case COLOUR_STATE::RED:
				color = vec4{ 1.f, 0.f, 0.f, 1.f };
			}
		}

	}

	registry.controls.clear();
	// TODO SnB9A38K: Move to player controller
	player_vel += (player_movement * player_speed);
	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

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
	{

		Entity player = Entity();

		// mark entity as player
		registry.players.emplace(player);

		// motion
		registry.positions.insert(player, vec2(WINDOW_WIDTH_PX * 0.5f, WINDOW_HEIGHT_PX * 0.5f));
		Size& sizePlayer = registry.sizes.emplace(player);
		sizePlayer.height = 20.f;
		sizePlayer.width = 20.f;
		registry.velocities.insert(player, vec2(0.0f, 0.0f));
		registry.angles.emplace(player);
		registry.collidables.insert(player, {false, true});

		// rendering so player is visible 
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

		// TODO m1EAwQf4: This is only being used to make sure that the mouse rectangle isn't being destroyed on world_system reset so maybe don't need this.
		// Recreate anything that may have been destroyed by world_system's restart_game function.
		auto center_screen = vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2);
		mouse_shape_entity = createRectangle(center_screen, center_screen + vec2(200));

		//the hovering rectangle now has a state associated with it (i am hoping this is how we are envisioning doing the code for walls
		//when placing the wall, it will have the state it did before, to test coloured collisions
		ColourState& csHR = registry.colourStates.emplace(mouse_shape_entity);
		csHR.prev_colour_state = COLOUR_STATE::NEUTRAL;
		csHR.curr_colour_state = COLOUR_STATE::NEUTRAL;

		// Now only one rectangle is collidable TODO: remove after testing
		Collidable& col = registry.collidables.get(mouse_shape_entity);
		col.inelastic = true;
		//registry.players.emplace(mouse_shape_entity);



		// Load level
		Entity load_request_entity;
		registry.loadRequests.insert(load_request_entity, { LOAD_TYPE::LOAD_LEVEL, current_level_uid });

		player_movement = vec2(0);
	}
}

// When entity collides with colour radius, it needs to have its behaviour change handled here
void WorldSystem::handle_behaviour_change() {

	ComponentContainer<ChangeBehaviour>& changes = registry.change_behaviours;
	for (uint i = 0; i < changes.components.size(); i++) {
		Entity& entity = changes.entities[i];
		std::cout << "Entity has interacted with a colour radius! Entity: " << entity.id() << std::endl;
		
		assert(!registry.players.entities.empty() && "There must be one player");
		Entity& player = registry.players.entities[0];
		assert(registry.colourStates.has(player) && "player entity must have a ColourState component");
		COLOUR_STATE player_colour_state = registry.colourStates.get(player).curr_colour_state;

		assert(registry.colourStates.has(entity) && "Entity that interacted with radius must have a ColourState component");
		ColourState& entity_colour_state = registry.colourStates.get(entity);
		entity_colour_state.prev_colour_state = entity_colour_state.curr_colour_state;
		entity_colour_state.curr_colour_state = player_colour_state;
		std::cout << "Entity's colour state changed to: " << int(entity_colour_state.curr_colour_state) << "from: " << int(entity_colour_state.prev_colour_state) << std::endl;
	}

	// Since behaviour change was handled, remove the entity
	registry.change_behaviours.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}