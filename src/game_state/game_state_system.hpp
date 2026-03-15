#pragma once
#include "ecs/system.hpp"
#include "game_state_constants.hpp"
#include <map>
#include <vector>
#include <set>

class GameStateSystem
{
public:
	void init();
	void step(float elapsed_ms);
	/// <summary>
	/// Register a system that always runs, regardless of the state the game is in.
	/// </summary>
	/// <param name="system">The system that will update at every step regardless of state.</param>
	void register_system(SystemInterface* system);
	void register_system(SystemInterface* system, GAME_STATE game_state);
	void register_system(SystemInterface* system, std::vector<GAME_STATE> game_states);
	void unregister_system(SystemInterface* system);
	void unregister_system(SystemInterface* system, GAME_STATE game_state);
	void unregister_system(SystemInterface* system, std::vector<GAME_STATE> game_states);

private:
	GAME_STATE current_state;
	std::set<SystemInterface*> always_active_systems;
	std::map<GAME_STATE, std::set<SystemInterface*>> state_systems;
};
