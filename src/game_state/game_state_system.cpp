#include "algorithm"
#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "game_state_constants.hpp"
#include "game_state_system.hpp"
#include <vector>

void GameStateSystem::init()
{
	current_state = INITIAL_GAME_STATE;

	// Initialize each set for each state
	for (int i = 0; i < static_cast<int>(GAME_STATE::GAME_STATE_COUNT); i++)
	{
		state_system_indices[static_cast<GAME_STATE>(i)] = {};
	}
}

void GameStateSystem::step(float elapsed_ms)
{
	if (registry.game_state_change_requests.size() > 0)
	{
		// If there are any requests to change the game state, take the most recent request only
		const Entity change_request_entity = registry.game_state_change_requests.entities.back();
		current_state = registry.game_state_change_requests.get(change_request_entity).game_state;
		// Remove the indices from the previous state
		current_system_indices.clear();
		std::vector<int> current_state_system_indices = state_system_indices[current_state];
		// Add both the general systems and the specific state systems
		current_system_indices.insert(current_system_indices.end(), always_active_system_indices.begin(), always_active_system_indices.end());
		current_system_indices.insert(current_system_indices.end(), current_state_system_indices.begin(), current_state_system_indices.end());
		// Sort so we go in order of registration
		std::sort(current_system_indices.begin(), current_system_indices.end());
		// Remove any other requests
		registry.game_state_change_requests.clear();
	}

	// Loop through only the current state systems
	for (int system_index : current_system_indices)
	{
		systems.at(system_index)->step(elapsed_ms);
	}
}

void GameStateSystem::register_system(SystemInterface* system)
{
	validate_systems(system);
	// Add to the always active and the current
	always_active_system_indices.push_back(systems.size());
	current_system_indices.push_back(systems.size());
	systems.push_back(system);
}

void GameStateSystem::register_system(SystemInterface* system, std::vector<GAME_STATE> game_states)
{
	validate_systems(system);

	for (GAME_STATE game_state : game_states)
	{
		state_system_indices[game_state].push_back(systems.size());

		// If the current state is the one we are registering, also add it to the current systems
		if (game_state == current_state)
		{
			current_system_indices.push_back(systems.size());
		}
	}

	// Add to the always active and the current
	systems.push_back(system);
}

void GameStateSystem::register_system(SystemInterface* system, GAME_STATE game_state)
{
	validate_systems(system);

	// If the current state is the one we are registering, also add it to the current systems
	if (game_state == current_state)
	{
		current_system_indices.push_back(systems.size());
	}

	state_system_indices[game_state].push_back(systems.size());
	systems.push_back(system);
}

void GameStateSystem::unregister_system(SystemInterface* system)
{
	auto system_pointer = std::find(systems.begin(), systems.end(), system);
	auto removed_index = system_pointer - systems.begin();

	// Reduce every int by 1 if it's bigger
	for (int& system_index : always_active_system_indices)
	{
		if (system_index > removed_index)
		{
			system_index--;
		}
	}

	// Initialize each set for each state
	for (int i = 0; i < static_cast<int>(GAME_STATE::GAME_STATE_COUNT); i++)
	{
		std::vector<int> system_indices = state_system_indices[static_cast<GAME_STATE>(i)];

		for (int& system_index : system_indices)
		{
			if (system_index > removed_index)
			{
				system_index--;
			}
		}
	}
}

void GameStateSystem::validate_systems(SystemInterface* system)
{
	// Make sure we only have 1 of each system
	for (auto& existing_system : systems)
	{
		assert(std::find(systems.begin(), systems.end(), system) == systems.end());
	}
}
