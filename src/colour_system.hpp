#pragma once

#include "ecs/entity.hpp"

class ColourSystem
{
public:
	ColourSystem();
	~ColourSystem();
	void on_collision(Entity collider, Entity collided);

private:

};

ColourSystem::ColourSystem()
{
}

ColourSystem::~ColourSystem()
{
}
