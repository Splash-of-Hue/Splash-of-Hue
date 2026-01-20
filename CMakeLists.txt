cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(Splash_of_Hue)

# generate the folder path location in a header file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ext/project_path.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/ext/project_path.hpp")

# add the executable
add_executable(Splash_of_Hue
				src/main.cpp
				src/systems.hpp
				src/invader_detection_system.cpp
				src/invader_movement_system.cpp
				src/invader_release_system.cpp
				src/load_entities.cpp
				src/tinyECS/components.hpp
				src/tinyECS/entity.hpp
				src/tinyECS/registry.hpp
				src/tinyECS/tiny_ecs.hpp
				src/tinyECS/tiny_ecs.cpp
				src/tinyECS/trace.h)

# fix visual studio startup project and structure
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT A0_tinyECS_tower)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
