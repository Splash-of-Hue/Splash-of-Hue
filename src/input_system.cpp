#include "input_system.hpp"
#include "world_system.hpp"
#include "world_init.hpp"

#include <map>
#include <sstream>
#include <iostream>

InputSystem::InputSystem()
{
}

GLFWwindow* InputSystem::init(GLFWwindow* window, WorldSystem* world_system)
{
	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html

	this->window = window;
	this->world_system = world_system;

	// Initialize movement keys
	move_keys_entity = Entity();
	registry.move_keys.emplace(move_keys_entity);

	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_pressed_redirect = [](GLFWwindow* wnd, int _button, int _action, int _mods) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button_pressed(_button, _action, _mods); };
	auto mouse_scroll_redirect = [](GLFWwindow* wnd, double _xoffset, double _yoffset) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_scroll(_xoffset, _yoffset); };
	auto window_moved_redirect = [](GLFWwindow* wnd, int _xpos, int _ypos) { ((InputSystem*)glfwGetWindowUserPointer(wnd))->on_window_moved(_xpos, _ypos); };

	glfwSetWindowPosCallback(window, window_moved_redirect);
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_pressed_redirect);
	glfwSetScrollCallback(window, mouse_scroll_redirect);

	return window;
}

// TODO m1EAwQf4: This is only being used to make sure that the mouse rectangle isn't being destroyed on world_system reset so maybe don't need this.
// Recreate anything that may have been destroyed by world_system's restart_game function.
void InputSystem::on_world_reset()
{
	auto center_screen = vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2);
	mouse_rect_entity = createRectangle(center_screen, center_screen + vec2(200));
}

// on key callback
void InputSystem::on_key(int key, int, int action, int mod) 
{
	MoveKeys& move_keys = registry.move_keys.get(move_keys_entity);

	// exit game w/ ESC
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		world_system->close_window();
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		world_system->restart_game();
		on_world_reset(); 
	}

	// Store shift modifier, if it's released it will be set to false, if it's pressed OR held down it will be true
	if (key == GLFW_KEY_LEFT_SHIFT)
	{
		shift_key_pressed = action != GLFW_RELEASE;
	}

	// Movement, on push sets the corresponding bitset value to true and on release sets it to false
	if (action == GLFW_PRESS || action == GLFW_RELEASE)
	{
		bool key_pressed = (action == GLFW_PRESS);

		switch (key)
		{
		case GLFW_KEY_W:
			move_keys.movement_keys.set(W, key_pressed);
			break;
		case GLFW_KEY_A:
			move_keys.movement_keys.set(A, key_pressed);
			break;
		case GLFW_KEY_S:
			move_keys.movement_keys.set(S, key_pressed);
			break;
		case GLFW_KEY_D:
			move_keys.movement_keys.set(D, key_pressed);
			break;
		default:
			break;
		}
	}

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		// TODO m1EAwQf4: Remove this when no longer need to manually make rectangles for test
		// Map numpad to increasing or decreasing the RGB value of rect on screen
		std::map<int, std::tuple<int, float>> color_rect_change = {
			{ GLFW_KEY_KP_1, { 0, -0.01 } },
			{ GLFW_KEY_KP_2, { 1, -0.01 } },
			{ GLFW_KEY_KP_3, { 2, -0.01 } },
			{ GLFW_KEY_KP_4, { 0,  0.01 } },
			{ GLFW_KEY_KP_5, { 1,  0.01 } },
			{ GLFW_KEY_KP_6, { 2,  0.01 } }
		};

		if (color_rect_change.count(key) > 0)
		{
			auto [rgb_index, value_change] = color_rect_change[key];
			auto& rect_color = registry.colors.get(mouse_rect_entity);
			rect_color[rgb_index] += value_change;
		}
	}

	// Debugging - not used in A1, but left intact for the debug lines
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE) {
			if (debugging.in_debug_mode) {
				debugging.in_debug_mode = false;
			}
			else {
				debugging.in_debug_mode = true;
			}
		}
	}
}

void InputSystem::on_mouse_move(vec2 mouse_position) {
	// record the current mouse position
	mouse_pos_x = mouse_position.x;
	mouse_pos_y = mouse_position.y;

	// TODO m1EAwQf4: Remove
	auto& pos = registry.positions.get(mouse_rect_entity);
	pos = mouse_position;
}

void InputSystem::on_window_moved(int xpos, int ypos)
{
	std::cout << "Window position, X: " << xpos << ", Y: " << ypos << std::endl;
}

void InputSystem::on_mouse_scroll(double xoffset, double yoffset)
{
	std::cout << "mouse scroll, X Offset: " << xoffset << ", Y Offset: " << yoffset << std::endl;

	// TODO m1EAwQf4: Remove
	auto& size = registry.sizes.get(mouse_rect_entity);
	if (shift_key_pressed)
	{
		size.width += yoffset;
	}
	else
	{
		size.height += yoffset;
	}
}

void InputSystem::on_mouse_button_pressed(int button, int action, int mods) {
	// on left button press
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {

		int tile_x = (int)(mouse_pos_x / GRID_CELL_WIDTH_PX);
		int tile_y = (int)(mouse_pos_y / GRID_CELL_HEIGHT_PX);

		std::cout << "mouse position: " << mouse_pos_x << ", " << mouse_pos_y << std::endl;
		std::cout << "mouse tile position: " << tile_x << ", " << tile_y << std::endl;

		// TODO m1EAwQf4: Remove
		auto& size = registry.sizes.get(mouse_rect_entity);
		auto& color = registry.colors.get(mouse_rect_entity);
		createRectangle(vec2(mouse_pos_x, mouse_pos_y), size.width, size.height, color);
	} else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {

		int tile_x = (int)(mouse_pos_x / GRID_CELL_WIDTH_PX);
		int tile_y = (int)(mouse_pos_y / GRID_CELL_HEIGHT_PX);

		std::cout << "CIRCLE mouse position: " << mouse_pos_x << ", " << mouse_pos_y << std::endl;
		std::cout << "CIRCLE mouse tile position: " << tile_x << ", " << tile_y << std::endl;

		// TODO m1EAwQf4: Remove
		auto& size = registry.sizes.get(mouse_rect_entity);
		auto& color = registry.colors.get(mouse_rect_entity);
		createCircle(vec2(mouse_pos_x, mouse_pos_y), size.width, color);
	}
}
