#pragma once
#include "save_constants.hpp"
#include <string>
#include <vector>

struct Savable
{
	// The ID to use for saving/loading
	std::string id;
	// All the things we should save
	std::vector<SAVE_TYPE> save_types;
};

struct SaveRequest
{

};