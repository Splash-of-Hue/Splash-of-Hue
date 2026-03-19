#include "creature_system/creature_creator.hpp"
#include "ecs/registry.hpp"
#include "shapes/shape_build_init.hpp"
#include "creature_system/creature_behaviour_tree_builder.hpp"
#include "creature_system/creature_components.hpp"
#include <iostream>

void CreatureCreator::addGeneralCreatureComponents(const int level_uid, Entity creature, vec2 position, float width, float height, COLOUR_STATE color_state, CREATURETYPE creature_type, TEXTURE_ASSET_ID texture_asset_id, bool shouldHaveInelasticCollision /* = true */) {
    const ColourPalette& palette = registry.colourPalettes.components[0];
    vec4 color = palette.state_to_colour.at(color_state);

    Collidable& collidable = registry.collidables.emplace(creature);
    collidable.inelastic = shouldHaveInelasticCollision;

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
    registry.levelEntities.insert(creature, { level_uid });
    registry.renderRequests.insert(creature, {
        EFFECT_ASSET_ID::TEXTURED,
        GEOMETRY_BUFFER_ID::SPRITE,
        RENDER_LAYER::CREATURES,
    });
}

Entity CreatureCreator::createAnglerfish(const int level_uid, vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity anglerfish = Entity();

    addGeneralCreatureComponents(level_uid, anglerfish, position, width, height, color_state, CREATURETYPE::ANGLERFISH, TEXTURE_ASSET_ID::ANGLERFISH_01);

    Pathfinder& pathfinder = registry.pathfinders.emplace(anglerfish);
    pathfinder.target = registry.players.entities[0];
    pathfinder.type = PATHFINDING_TYPE::BEST;
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(anglerfish);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildAnglerfishTree();
   
    return anglerfish;
}

Entity CreatureCreator::createCrab(const int level_uid, vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity crab = Entity();

    // TODO add dedicated crab asset and texture ID
	addGeneralCreatureComponents(level_uid, crab, position, width, height, color_state, CREATURETYPE::CRAB, TEXTURE_ASSET_ID::CREATURES);
    
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(crab);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildCrabTree();
    
    return crab;
}

Entity CreatureCreator::createMinnows(const int level_uid, vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity minnows = Entity();
    
	addGeneralCreatureComponents(level_uid, minnows, position, width, height, color_state, CREATURETYPE::MINNOWS, TEXTURE_ASSET_ID::MINNOW_01);
    registry.renderRequests.remove(minnows); // Make sure the minnow overseer is not seen

    BoidOverseer& minnow_overseer = registry.boidOverseers.emplace(minnows);
    
    for (int i = 0; i < NUM_MINNOWS; i++)
    {
        Entity minnow_follower;
        registry.boid_minnows.emplace(minnow_follower);

        addGeneralCreatureComponents(level_uid, minnow_follower, position, width, height, color_state, CREATURETYPE::MINNOWS, TEXTURE_ASSET_ID::MINNOW_01);

        Collidable& collidable = registry.collidables.get(minnow_follower);
        collidable.inelastic = true; // sophia note: we can change this to true
        collidable.overlay = false;

        vec2& scaleminnow = registry.scales.get(minnow_follower);
        scaleminnow = vec2(0.5f * GRID_CELL_WIDTH_PX, 0.5f * GRID_CELL_HEIGHT_PX);
    }


    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(minnows);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildMinnowTree();
    
    return minnows;
}

// Todo: AkWAGMV1 cleanup and make just createArm, cleanup hardcoded values
// Todo: ojg8otlR, replace rectangle with polygon mesh
// Todo: SibWWzX3, set arms to be invisible
// For the sake of simplicity + time for M2, just use the shrimp width to make a rectangle
Entity CreatureCreator::createLeftArm(vec2 shrimp_position, float shrimp_width, vec4 colour)
{
    float left_arm_x = shrimp_position.x - SHRIMP_ARM_TRANSLATION_X;
    float left_arm_y = shrimp_position.y + 5.0f;

    Entity& armEntity = createRectangle(vec2(left_arm_x, left_arm_y), shrimp_width, shrimp_width / 4, RENDER_LAYER::CREATURES, colour);
    registry.collidables.emplace(armEntity);
    return armEntity;
}

// Todo: AkWAGMV1 cleanup and make just createArm, cleanup hardcoded values
// Todo: ojg8otlR, replace rectangle with polygon mesh
// Todo: SibWWzX3, set arms to be invisible
// For the sake of simplicity + time for M2, just use the shrimp width to make a rectangle
Entity CreatureCreator::createRightArm(vec2 shrimp_position, float shrimp_width, vec4 colour)
{
    float right_arm_x = shrimp_position.x + SHRIMP_ARM_TRANSLATION_X;
    float right_arm_y = shrimp_position.y + 5.0f;

    Entity& armEntity = createRectangle(vec2(right_arm_x, right_arm_y), shrimp_width, shrimp_width / 4, RENDER_LAYER::CREATURES, colour);
    registry.collidables.emplace(armEntity);
    return armEntity;
}

// Todo: AkWAGMV1 cleanup and make just createArm, cleanup hardcoded values
// Todo: ojg8otlR, replace rectangle with polygon mesh
// Todo: SibWWzX3, set arms to be invisible
// For the sake of simplicity + time for M2, just use the shrimp width to make a rectangle
Entity CreatureCreator::createUpArm(vec2 shrimp_position, float shrimp_width, vec4 colour)
{
    float up_arm_x = shrimp_position.x + 10.0f;
    float up_arm_y = shrimp_position.y - SHRIMP_ARM_TRANSLATION_Y;

    Entity& armEntity = createRectangle(vec2(up_arm_x, up_arm_y), shrimp_width / 4, shrimp_width - (shrimp_width / 4), RENDER_LAYER::CREATURES, colour);
    registry.collidables.emplace(armEntity);
    return armEntity;
}

// Todo: AkWAGMV1 cleanup and make just createArm, cleanup hardcoded values
// Todo: ojg8otlR, replace rectangle with polygon mesh
// Todo: SibWWzX3, set arms to be invisible
// For the sake of simplicity + time for M2, just use the shrimp width to make a rectangle
Entity CreatureCreator::createDownArm(vec2 shrimp_position, float shrimp_width, vec4 colour)
{
    float down_arm_x = shrimp_position.x - 10.0;
    float down_arm_y = shrimp_position.y + SHRIMP_ARM_TRANSLATION_Y + 20.0f; // translate a bit extra to accommodate for body

    Entity& armEntity = createRectangle(vec2(down_arm_x, down_arm_y), shrimp_width / 4, shrimp_width - (shrimp_width / 4), RENDER_LAYER::CREATURES, colour);
    registry.collidables.emplace(armEntity);
    return armEntity;
}

// Todo: AkWAGMV1 cleanup and turn into an iterator within createShrimp
/*
void createThreeArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height)
{


}
*/

// Todo: AkWAGMV1 cleanup and turn into an iterator within createShrimp
void CreatureCreator::createTwoArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    switch (curr_direction_facing)
    {
    case DIRECTION::UP:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createUpArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createUpArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createRightArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createDownArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    case DIRECTION::DOWN:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createDownArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createRightArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createDownArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createUpArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createRightArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    case DIRECTION::LEFT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createDownArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createUpArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createDownArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createRightArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    case DIRECTION::RIGHT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createUpArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createRightArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createDownArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createRightArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createUpArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    default:
        assert(false && "direction passed to createShrimp is not a valid DIRECTION");
    }
}

// Todo: AkWAGMV1 cleanup and turn into an iterator within createShrimp
void CreatureCreator::createOneArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    switch (curr_direction_facing)
    {
    case DIRECTION::UP:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createUpArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createRightArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    case DIRECTION::DOWN:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createDownArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createRightArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    case DIRECTION::LEFT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createLeftArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createUpArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createDownArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    case DIRECTION::RIGHT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createRightArm(shrimp_position, shrimp_width, vec4(BLACK_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createDownArm(shrimp_position, shrimp_width, vec4(BLUE_COLOUR, 0.0f)));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createUpArm(shrimp_position, shrimp_width, vec4(RED_COLOUR, 0.0f)));

        break;

    default:
        assert(false && "direction passed to createShrimp is not a valid DIRECTION");
    }
}

Entity CreatureCreator::createShrimp(const int level_uid, vec2 position, float width, float height, COLOUR_STATE color_state,  DIRECTION curr_direction_facing /* = UP */, int num_arms /* = 2 */)
{
    Entity shrimp = Entity();

    addGeneralCreatureComponents(level_uid, shrimp, position, width, height, color_state, CREATURETYPE::SHRIMP, TEXTURE_ASSET_ID::PISTOL_SHRIMP_01, false);
    registry.scales.get(shrimp) = vec2(5.0f * GRID_CELL_WIDTH_PX, 5.0f * GRID_CELL_HEIGHT_PX);

    Shrimp& shrimp_component = registry.shrimps.emplace(shrimp); // direction tracking component
    assert((num_arms == 1 || num_arms == 2) && "shrimp can only be initialized with 1, or 2 arms at this time");
    shrimp_component.num_arms = num_arms;

    // Todo: AkWAGMV1 cleanup and turn switch into an iterator
    // Add appropriate arms for this shrimp
    switch (num_arms)
    {
    case 1:
        createOneArmShrimp(shrimp_component, curr_direction_facing, position, width, height);
        break;

    case 2:
        createTwoArmShrimp(shrimp_component, curr_direction_facing, position, width, height);
        break;

    case 3:
        //createThreeArmShrimp(shrimp_component, curr_direction_facing, position, width, height); // TODO: 0aEQcx13
        break;
    }    

    // Todo: AkWAGMV1 cleanup and just add the component every time we createArm instead of this
    // Add the arms to the level entities so they'll be unloaded later
    for (auto& [colour, arm_entities] : shrimp_component.shrimp_arms)
    {
        for (Entity& arm_entity : arm_entities)
        {
            LevelEntity& arm_level_entity = registry.levelEntities.emplace(arm_entity);
            arm_level_entity.level_uid = level_uid;
        }
    }

    ANIMATABLE_TYPE shrimp_anim_type;
    switch (curr_direction_facing)
    {
        case DIRECTION::UP:    
            shrimp_anim_type = ANIMATABLE_TYPE::PISTOL_SHRIMP_UP;    
            break;
        case DIRECTION::DOWN:  
            shrimp_anim_type = ANIMATABLE_TYPE::PISTOL_SHRIMP_DOWN;  
            break;
        case DIRECTION::LEFT:  
            shrimp_anim_type = ANIMATABLE_TYPE::PISTOL_SHRIMP_LEFT;  
            break;
        case DIRECTION::RIGHT: 
            shrimp_anim_type = ANIMATABLE_TYPE::PISTOL_SHRIMP_RIGHT; 
            break;
        default:               
            shrimp_anim_type = ANIMATABLE_TYPE::PISTOL_SHRIMP_UP;    
            break;
    }

    const Animatable animatable = {
        shrimp_anim_type,
        ANIMATION_BEHAVIOR::LOOP,
        ANIMATION_TRIGGER::SHRIMP_NEUTRAL,
        4
    };
    registry.animatables.insert(shrimp, animatable);

    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(shrimp);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildShrimpTree();
    
    return shrimp;
}

Entity CreatureCreator::createSawfish(const int level_uid, vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity sawfish = Entity();

	addGeneralCreatureComponents(level_uid, sawfish, position, width, height, color_state, CREATURETYPE::SAWFISH, TEXTURE_ASSET_ID::SAWFISH);

    Pathfinder& pathfinder = registry.pathfinders.emplace(sawfish);
    pathfinder.target = registry.players.entities[0];
    pathfinder.type = PATHFINDING_TYPE::BEST;
    pathfinder.max_speed = 500.0f;
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(sawfish);
    creatureBehaviour.behaviour_tree = CreatureBehaviourTreeBuilder::buildSawfishTree();
    const Animatable animatable = {
        ANIMATABLE_TYPE::SAWFISH,
        ANIMATION_BEHAVIOR::LOOP,
        ANIMATION_TRIGGER::IDLE,
        1
    };
    registry.animatables.insert(sawfish, animatable);
   
    return sawfish;
}

Entity CreatureCreator::createWhale(const int level_uid, vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity whale = Entity();

	addGeneralCreatureComponents(level_uid, whale, position, width, height, color_state, CREATURETYPE::WHALE, TEXTURE_ASSET_ID::WHALE_01);

    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(whale);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildWhaleTree();
    
    return whale;
}
