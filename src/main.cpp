#include <iostream>
#include<string>

#include "../ext/project_path.hpp"

#include "tinyECS/tiny_ecs.hpp"
#include "tinyECS/registry.hpp"

// forward function declarations
#include "systems.hpp"
// #include "load_entities.cpp"
bool load_entities(std::string& filename, Registry& entity_registry);

//
// main
//
int main(int argc, char* argv[])
{
	Registry entity_registry;

	// default filename, with a command line override below
	std::string filename = PROJECT_SOURCE_DIR + std::string("tests//sample_scenario.txt");

	// override the filename with a command line to support testing with other configurations
	if (argc > 1) {
		filename = argv[1];
	}

	if (!load_entities(filename, entity_registry))
	{
		std::cout << "ERROR: failed to load entities from file: " << filename << "." << std::endl;
		return -1;
	}

	// hardcoded max-x value (don't change for A0)
	float MAX_X = 11;

	std::cout << "========================================" << std::endl;
	std::cout << "Starting game loop." << std::endl;
	std::cout << "========================================" << std::endl;

	// NOTE: you cannot the system calls in like this
	/*
	while (invader_release_system(entity_registry) > 0
		|| invader_movement_system(entity_registry, MAX_X) > 0
		|| invader_detection_system(entity_registry) > 0) {

	... because the first system will keep running until it is done rather than interleaving as it should
	*/
	int num_systems_to_process = 1;	// start with 1 so the loop runs first time
	while (num_systems_to_process > 0) {

		// sum system counts
		num_systems_to_process = 0;
		num_systems_to_process += invader_release_system(entity_registry);
		num_systems_to_process += invader_movement_system(entity_registry, MAX_X);
		num_systems_to_process += invader_detection_system(entity_registry);
		
		// break between system rounds
		std::cout << "========================================" << std::endl;
	}

	std::cout << "========================================" << std::endl;
	std::cout << "Ending game loop." << std::endl;
	std::cout << "========================================" << std::endl;

	return EXIT_SUCCESS;
}
