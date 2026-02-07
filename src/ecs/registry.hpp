#pragma once
#include <vector>

#include "animation/animation_components.hpp"
#include "renderer/render_components.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

class ECSRegistry
{
	// callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Eatable> eatables;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec4> colors;
	ComponentContainer<Tower> towers;
	ComponentContainer<GridLine> gridLines;
	ComponentContainer<Invader> invaders;
	ComponentContainer<Projectile> projectiles;
	ComponentContainer<vec2> positions;
	ComponentContainer<Angle> angles;
	ComponentContainer<vec2> scales;
	ComponentContainer<Size> sizes;
	ComponentContainer<Rect> rectangles; // TODO 1ch6Q0BE: Remove
	ComponentContainer<Circle> circles; // TODO 1ch6Q0BE: Remove
	ComponentContainer<OverlayCollision> overlayCollisions;
	ComponentContainer<InelasticCollision> inelasticCollisions;
	ComponentContainer<MoveKeys> move_keys; // There should only be 1 entity for move_keys... TODO better way to store it then?
	ComponentContainer<Colour> colours;
	ComponentContainer<ColourWheel> colourWheels;
	ComponentContainer<ColourPalette> colourPalettes;
	ComponentContainer<ColourMenuState> colourMenuStates;
	ComponentContainer<ColourMenuInput> colourMenuInputs;
	ComponentContainer<Deadly> deadlys;
	ComponentContainer<Animatable> animatables;
	ComponentContainer<Animation> animations;
	ComponentContainer<AnimationTrigger> animation_triggers;
	ComponentContainer<Textured> textured;

	// constructor that adds all containers for looping over them
	ECSRegistry()
	{
		
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&eatables);
		registry_list.push_back(&deadlys);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&towers);
		registry_list.push_back(&gridLines);
		registry_list.push_back(&invaders);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&positions);
		registry_list.push_back(&angles);
		registry_list.push_back(&scales);
		registry_list.push_back(&sizes);
		registry_list.push_back(&rectangles); // TODO 1ch6Q0BE: Remove
		registry_list.push_back(&circles); // TODO 1ch6Q0BE: Remove
		registry_list.push_back(&overlayCollisions);
		registry_list.push_back(&inelasticCollisions);
		registry_list.push_back(&animatables);
		registry_list.push_back(&animations);
		registry_list.push_back(&animation_triggers);
		registry_list.push_back(&textured);
		registry_list.push_back(&colours);
		registry_list.push_back(&colourWheels);
		registry_list.push_back(&colourPalettes);
		registry_list.push_back(&colourMenuStates);
		registry_list.push_back(&colourMenuInputs);
	}

	void clear_all_components()
	{
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components()
	{
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e)
	{
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e)
	{
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;
