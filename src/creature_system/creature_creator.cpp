#include "creature_system/creature_creator.hpp"
#include "ecs/registry.hpp"
#include "world_init.hpp"
#include "creature_system/creature_behaviour_tree_builder.hpp"
#include "creature_system/creature_components.hpp"

void addGeneralCreatureComponents(Entity creature, vec2 position, float width, float height, COLOUR_STATE color_state, CREATURETYPE creature_type, TEXTURE_ASSET_ID texture_asset_id) {
    const ColourPalette& palette = registry.colourPalettes.components[0];
    vec4 color = palette.state_to_colour.at(color_state);

    Collidable& collidable = registry.collidables.emplace(creature);
    collidable.inelastic = true;

    Textured& textured = registry.textured.emplace(creature);
    textured.used_texture = texture_asset_id;

    registry.angles.emplace(creature);
    registry.angles.get(creature).degrees = 0.0f;
    registry.colors.emplace(creature, color);
    registry.colourStates.insert(creature, { color_state });
    registry.creatures.insert(creature, { creature_type });
    registry.positions.insert(creature, position);
    registry.scales.insert(creature, vec2(2.0f * GRID_CELL_WIDTH_PX, 2.0f * GRID_CELL_HEIGHT_PX));
    registry.sizes.insert(creature, Size{ width, height });
    registry.velocities.emplace(creature);
    
    registry.renderRequests.insert(creature, {
        EFFECT_ASSET_ID::TEXTURED,
        GEOMETRY_BUFFER_ID::SPRITE
    });
}

Entity createAnglerfish(vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity anglerfish = Entity();

    addGeneralCreatureComponents(anglerfish, position, width, height, color_state, CREATURETYPE::ANGLERFISH, TEXTURE_ASSET_ID::ANGLERFISH_01);

    Pathfinder& pathfinder = registry.pathfinders.emplace(anglerfish);
    pathfinder.target = registry.players.entities[0];
    pathfinder.type = PATHFINDING_TYPE::BEST;
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(anglerfish);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildAnglerfishTree();
   
    return anglerfish;
}

Entity createCrab(vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity crab = Entity();

    // TODO add dedicated crab asset and texture ID
	addGeneralCreatureComponents(crab, position, width, height, color_state, CREATURETYPE::CRAB, TEXTURE_ASSET_ID::CREATURES);
    
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(crab);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildCrabTree();
    
    return crab;
}

Entity createMinnows(vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity minnows = Entity();
    
	addGeneralCreatureComponents(minnows, position, width, height, color_state, CREATURETYPE::MINNOWS, TEXTURE_ASSET_ID::MINNOW_01);

    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(minnows);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildMinnowTree();
    
    return minnows;
}

Entity createShrimp(vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity shrimp = Entity();

    addGeneralCreatureComponents(shrimp, position, width, height, color_state, CREATURETYPE::SHRIMP, TEXTURE_ASSET_ID::PISTOL_SHRIMP_01);

    registry.shrimps.emplace(shrimp); // direction tracking component
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(shrimp);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildShrimpTree();
    
    return shrimp;
}

Entity createSawfish(vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity sawfish = Entity();

	addGeneralCreatureComponents(sawfish, position, width, height, color_state, CREATURETYPE::SAWFISH, TEXTURE_ASSET_ID::SAWFISH_01);

    Pathfinder& pathfinder = registry.pathfinders.emplace(sawfish);
    pathfinder.target = registry.players.entities[0];
    pathfinder.type = PATHFINDING_TYPE::BEST;
    pathfinder.max_speed = 6000.0f;
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(sawfish);
    creatureBehaviour.behaviour_tree = CreatureBehaviourTreeBuilder::buildSawfishTree();
   
    return sawfish;
}

Entity createWhale(vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity whale = Entity();

	addGeneralCreatureComponents(whale, position, width, height, color_state, CREATURETYPE::WHALE, TEXTURE_ASSET_ID::WHALE_01);

    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(whale);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildWhaleTree();
    
    return whale;
}
