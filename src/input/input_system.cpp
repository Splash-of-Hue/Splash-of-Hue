#include "input/input_system.hpp"
#include <iostream>
#include <string>

// TODO H0TP8yUY: Known issue where connecting controller breaks because the pointer is not set
// Disconnecting clears the pointer as well
void InputSystem::on_joystick_connection_changed(int jid, int event)
{
	const auto joystick_is_gamepad = glfwJoystickIsGamepad(jid);
	const auto& name = get_connected_controller_name(jid, joystick_is_gamepad);
	const auto connected_set = joystick_is_gamepad ? &connected_gamepads : &connected_joysticks;
	const auto& type_string = joystick_is_gamepad ? "Gamepad " : "Joystick ";

	if (event == GLFW_CONNECTED)
	{
		// The joystick was connected
		connected_set->insert(jid);
		pressed_controller_buttons[jid] = {};
		std::cout << type_string << name << " connected, jid: " << jid << ", event: " << event << std::endl;
	}
	else if (event == GLFW_DISCONNECTED)
	{
		// The joystick was disconnedcted
		connected_set->erase(jid);
		glfwSetJoystickUserPointer(jid, this);
		pressed_controller_buttons[jid].clear();
		std::cout << type_string << name << " disconnected, jid: " << jid << ", event: " << event << std::endl;
	}
}

const std::string InputSystem::get_connected_controller_name(int jid, bool is_gamepad)
{
	if (controller_names.count(jid) > 0)
	{
		return controller_names[jid];
	}

	auto present = glfwJoystickPresent(jid);

	if (present == GLFW_FALSE)
	{
		return "Controller";
	}

	const char* controller_name;

	if (is_gamepad)
	{
		controller_name = glfwGetGamepadName(jid);
	}
	else
	{
		controller_name = glfwGetJoystickName(jid);
	}

	controller_names[jid] = controller_name;
	return controller_name;
}

// TODO LtiFv3Nh: Move to a window system
void InputSystem::on_window_moved(int xpos, int ypos)
{
	std::cout << "Window position, X: " << xpos << ", Y: " << ypos << std::endl;
}
