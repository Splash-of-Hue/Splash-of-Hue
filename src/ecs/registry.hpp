#pragma once
#include <vector>

#include "animation/animation_components.hpp"
#include "camera/camera_components.hpp"
#include "components.hpp"
#include "controllers/player_components.hpp"
#include "controls/control_components.hpp"
#include "creature_system/creature_components.hpp"
#include "input/input_components.hpp"
#include "interactables/interactable_components.hpp"
#include "loading/loading_components.hpp"
#include "map/map_components.hpp"
#include "renderer/render_components.hpp"
#include "text/text_components.hpp"

class ECSRegistry
{
	// callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// AI
	ComponentContainer<Charging> chargers;
	ComponentContainer<CreatureBehaviour> creatureBehaviours;
	ComponentContainer<CreatureBehaviourCooldown> creatureBehaviourCooldowns;
	ComponentContainer<CreatureBehaviourTimer> creatureBehaviourTimers;
	ComponentContainer<IsInInelasticCollision> creatureInelasticCollisions;
	ComponentContainer<Dizzy> dizzies;
	ComponentContainer<Pathfinder> pathfinders;
	// Animation
	ComponentContainer<Animatable> animatables;
	ComponentContainer<AnimationTrigger> animation_triggers;
	ComponentContainer<Animation> animations;
	ComponentContainer<Textured> textured;
	// Colour System
	ComponentContainer<vec4> colors;
	ComponentContainer<ColourState> colourStates;
	ComponentContainer<ColourWheel> colourWheels;
	ComponentContainer<ColourPalette> colourPalettes;
	ComponentContainer<ColourMenuState> colourMenuStates;
	ComponentContainer<ColourMenuInput> colourMenuInputs;
	ComponentContainer<LightEmission> lightEmissions;
	ComponentContainer<LightSource> lightSources;
	// Controls
	ComponentContainer<Control> controls;
	ComponentContainer<Input> inputs;
	ComponentContainer<InputClearRequest> inputClearRequests;
	// Debug
	ComponentContainer<DebugComponent> debugComponents;
	// Interactables
	ComponentContainer<Box> boxes;
	ComponentContainer<Breakable> breakables;
	ComponentContainer<Collectable> collectables;
	ComponentContainer<ColourOrb> colourOrbs;
	ComponentContainer<Interactable> interactables;
	ComponentContainer<MossToggle> mossToggles;
	ComponentContainer<Mushroom> mushrooms;	
	ComponentContainer<OrbCollected> orbCollected;
	// Loader
	ComponentContainer<LoadRequest> loadRequests;
	// Map
	ComponentContainer<ChangeMapLevelRequest> changeMapLevelRequests;
	ComponentContainer<ColouredWall> colouredWall;
	ComponentContainer<CurrentLevel> currentLevels;
	ComponentContainer<LevelEntity> levelEntities;
	ComponentContainer<Level> levels;
	ComponentContainer<LevelTile> levelTiles;
	ComponentContainer<MapTile> mapTiles;
	ComponentContainer<Teleporter> teleporters;
	// Physics - Collisions
	ComponentContainer<ChangeBehaviour> change_behaviours;
	ComponentContainer<Collidable> collidables;
	ComponentContainer<Collision> collisions;
	ComponentContainer<PolygonMesh> hitboxes;
	ComponentContainer<PolygonMesh> hurtboxes;
	ComponentContainer<InRadius> inRadius;
	// Physics - Movement / Transforms
	ComponentContainer<Angle> angles;
	ComponentContainer<vec2> positions;
	ComponentContainer<vec2> scales;
	ComponentContainer<Size> sizes;
	ComponentContainer<vec2> velocities;
	// Player
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<PlayerCreateRequests> playerCreateRequests;
	ComponentContainer<Player> players;
	// Rendering
	ComponentContainer<Camera> cameras;
	ComponentContainer<ChangeCameraTargetRequest> changeCameraTargetRequests;
	ComponentContainer<IgnoresCamera> ignoredByCamera;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<RenderSourceRect> renderSourceRects;
	ComponentContainer<ScreenState> screenStates;
	// Shapes
	ComponentContainer<Circle> circles; // TODO 1ch6Q0BE: Remove
	ComponentContainer<Rect> rectangles; // TODO 1ch6Q0BE: Remove
	// Spawnables
	ComponentContainer<Creature> creatures;
	ComponentContainer<Shrimp> shrimps;
	ComponentContainer<Sawfish> sawfishes;
	ComponentContainer<Text> texts;

	// constructor that adds all containers for looping over them
	ECSRegistry()
	{
		// AI
		registry_list.push_back(&chargers);
		registry_list.push_back(&creatureBehaviours);
		registry_list.push_back(&creatureBehaviourCooldowns);
		registry_list.push_back(&creatureBehaviourTimers);
		registry_list.push_back(&creatureInelasticCollisions);
		registry_list.push_back(&dizzies);
		registry_list.push_back(&pathfinders);
		// Animation
		registry_list.push_back(&animatables);
		registry_list.push_back(&animation_triggers);
		registry_list.push_back(&animations);
		registry_list.push_back(&textured);
		// Colour System
		registry_list.push_back(&colors);
		registry_list.push_back(&colourStates);
		registry_list.push_back(&colourWheels);
		registry_list.push_back(&colourPalettes);
		registry_list.push_back(&colourMenuStates);
		registry_list.push_back(&colourMenuInputs);
		registry_list.push_back(&lightEmissions);
		registry_list.push_back(&lightSources);
		// Controls
		registry_list.push_back(&controls);
		registry_list.push_back(&inputs);
		registry_list.push_back(&inputClearRequests);
		// Debug
		registry_list.push_back(&debugComponents);
		// Interactables
		registry_list.push_back(&boxes);
		registry_list.push_back(&breakables);
		registry_list.push_back(&collectables);
		registry_list.push_back(&colourOrbs);
		registry_list.push_back(&interactables);
		registry_list.push_back(&mossToggles);
		registry_list.push_back(&mushrooms);
		registry_list.push_back(&orbCollected);
		// Loader
		registry_list.push_back(&loadRequests);
		// Map
		registry_list.push_back(&changeMapLevelRequests);
		registry_list.push_back(&colouredWall);
		registry_list.push_back(&currentLevels);
		registry_list.push_back(&levelEntities);
		registry_list.push_back(&levels);
		registry_list.push_back(&levelTiles);
		registry_list.push_back(&mapTiles);
		registry_list.push_back(&teleporters);
		// Physics - Collisions
		registry_list.push_back(&change_behaviours);
		registry_list.push_back(&collidables);
		registry_list.push_back(&collisions);
		registry_list.push_back(&hitboxes);
		registry_list.push_back(&hurtboxes);
		registry_list.push_back(&inRadius);
		// Physics - Movement / Transforms
		registry_list.push_back(&angles);
		registry_list.push_back(&positions);
		registry_list.push_back(&scales);
		registry_list.push_back(&sizes);
		registry_list.push_back(&velocities);
		// Player
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&playerCreateRequests);
		registry_list.push_back(&players);
		// Rendering
		registry_list.push_back(&cameras);
		registry_list.push_back(&changeCameraTargetRequests);
		registry_list.push_back(&ignoredByCamera);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&renderSourceRects);
		registry_list.push_back(&screenStates);
		// Shapes
		registry_list.push_back(&circles); // TODO 1ch6Q0BE: Remove;
		registry_list.push_back(&rectangles); // TODO 1ch6Q0BE: Remove
		// Spawnables
		registry_list.push_back(&creatures);
		registry_list.push_back(&shrimps);
		registry_list.push_back(&sawfishes);
		// UI
		registry_list.push_back(&texts);
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
