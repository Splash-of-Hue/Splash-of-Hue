#pragma once
#include "ecs/system.hpp"
#include "game_state_constants.hpp"
#include <map>
#include <set>
#include <vector>

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
	/// <summary>
	/// Register a system that runs during a single state of the game.
	/// </summary>
	/// <param name="system">The system that will update when in the specified state.</param>
	/// <param name="game_state">The state the system will run in.</param>
	void register_system(SystemInterface* system, GAME_STATE game_state);
	/// <summary>
	/// Register a system that runs during multiple, but not all, states of the game.
	/// </summary>
	/// <param name="system">The system that will update when in any of the specified states.</param>
	/// <param name="game_states">The list of states the systen will run in.</param>
	void register_system(SystemInterface* system, std::vector<GAME_STATE> game_states);
	void unregister_system(SystemInterface* system);
	void unregister_system(SystemInterface* system, GAME_STATE game_state);
	void unregister_system(SystemInterface* system, std::vector<GAME_STATE> game_states);

private:
	GAME_STATE current_state;
	std::set<SystemInterface*> always_active_systems;
	std::map<GAME_STATE, std::set<SystemInterface*>> state_systems;
};
