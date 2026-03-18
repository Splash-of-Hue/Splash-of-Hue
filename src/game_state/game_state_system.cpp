#include "common.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"
#include "game_state_constants.hpp"
#include "game_state_system.hpp"
#include <set>

void GameStateSystem::init()
{
	current_state = INITIAL_GAME_STATE;

	// Initialize each set for each state
	for (int i = 0; i < static_cast<int>(GAME_STATE::GAME_STATE_COUNT); i++)
	{
		state_systems[(GAME_STATE)i] = {};
	}
}

void GameStateSystem::step(float elapsed_ms)
{
	if (registry.game_state_change_requests.size() > 0)
	{
		// If there are any requests to change the game state, take the most recent request only
		const Entity change_request_entity = registry.game_state_change_requests.entities.back();
		current_state = registry.game_state_change_requests.get(change_request_entity).game_state;
		registry.game_state_change_requests.clear();
	}

	// Loop through only the active state systems
	std::set<SystemInterface*>& active_state_systems = state_systems.at(current_state);

	for (SystemInterface* active_state_system : active_state_systems)
	{
		active_state_system->step(elapsed_ms);
	}

	// Then loop through the systems that are always active
	for (SystemInterface* always_active_system: always_active_systems)
	{
		always_active_system->step(elapsed_ms);
	}
}

void GameStateSystem::register_system(SystemInterface* system)
{
	// Don't register a system to run all the time if we have already registered it to run only some times
	for (auto& system_state_pair : state_systems)
	{
		assert(system_state_pair.second.count(system) == 0);
	}

	always_active_systems.insert(system);
}

void GameStateSystem::register_system(SystemInterface* system, std::vector<GAME_STATE> game_states)
{
	for (GAME_STATE game_state : game_states)
	{
		register_system(system, game_state);
	}
}

void GameStateSystem::register_system(SystemInterface* system, GAME_STATE game_state)
{
	// If we are trying to register a system to only run that we have already registered to always run, there is an error
	assert(always_active_systems.count(system) == 0);
	state_systems.at(game_state).insert(system);
}

void GameStateSystem::unregister_system(SystemInterface* system)
{
	always_active_systems.erase(system);
}

void GameStateSystem::unregister_system(SystemInterface* system, std::vector<GAME_STATE> game_states)
{
	for (GAME_STATE game_state : game_states)
	{
		unregister_system(system, game_state);
	}
}

void GameStateSystem::unregister_system(SystemInterface* system, GAME_STATE game_state)
{
	state_systems.at(game_state).erase(system);
}
