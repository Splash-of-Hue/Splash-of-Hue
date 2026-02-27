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

    registry.pathfinders.emplace(anglerfish);
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

// shrimp arms like this:

Entity createLeftArm(vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    float left_arm_x = shrimp_position.x - SHRIMP_ARM_TRANSLATION_X;
    float left_arm_y = shrimp_position.y;

    return createRectangle(vec2(shrimp_position.x, shrimp_position.y), shrimp_height, shrimp_height / 2, BLUE_COLOUR_VEC4);
}

Entity createRightArm(vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    float left_arm_x = shrimp_position.x + SHRIMP_ARM_TRANSLATION_X;
    float left_arm_y = shrimp_position.y;

    return createRectangle(vec2(shrimp_position.x, shrimp_position.y), shrimp_height, shrimp_height / 2, BLUE_COLOUR_VEC4);
}

Entity createUpArm(vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    float left_arm_x = shrimp_position.x;
    float left_arm_y = shrimp_position.y - SHRIMP_ARM_TRANSLATION_Y;

    return createRectangle(vec2(shrimp_position.x, shrimp_position.y), shrimp_height / 2, shrimp_height, BLUE_COLOUR_VEC4);
}

Entity createDownArm(vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    float left_arm_x = shrimp_position.x;
    float left_arm_y = shrimp_position.y + SHRIMP_ARM_TRANSLATION_Y;

    return createRectangle(vec2(shrimp_position.x, shrimp_position.y), shrimp_height / 2, shrimp_height, BLUE_COLOUR_VEC4);
}

void createThreeArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height)
{


}


void createTwoArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    switch (curr_direction_facing)
    {
    case DIRECTION::UP:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    case DIRECTION::DOWN:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    case DIRECTION::LEFT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    case DIRECTION::RIGHT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    default:
        assert(false && "direction passed to createShrimp is not a valid DIRECTION");
    }
}


void createOneArmShrimp(Shrimp& shrimp_component, DIRECTION curr_direction_facing, vec2 shrimp_position, float shrimp_width, float shrimp_height)
{
    switch (curr_direction_facing)
    {
    case DIRECTION::UP:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    case DIRECTION::DOWN:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    case DIRECTION::LEFT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createLeftArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    case DIRECTION::RIGHT:
        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::NEUTRAL].push_back(createRightArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::BLUE].push_back(createDownArm(shrimp_position, shrimp_width, shrimp_height));

        shrimp_component.shrimp_arms[SHRIMP_ARM_COLOUR::RED].push_back(createUpArm(shrimp_position, shrimp_width, shrimp_height));

        break;

    default:
        assert(false && "direction passed to createShrimp is not a valid DIRECTION");
    }
}

Entity createShrimp(vec2 position, float width, float height, COLOUR_STATE color_state, DIRECTION curr_direction_facing /* = UP */, int num_arms /* = 2 */)
{
    Entity shrimp = Entity();

    addGeneralCreatureComponents(shrimp, position, width, height, color_state, CREATURETYPE::SHRIMP, TEXTURE_ASSET_ID::PISTOL_SHRIMP_01);

    Shrimp& shrimp_component = registry.shrimps.emplace(shrimp); // direction tracking component
    assert((num_arms == 1 || num_arms == 2 || num_arms == 3) && "shrimp can only be initialized with 1, 2, or 3 arms at this time");
    shrimp_component.num_arms = num_arms;

    createCircle(position, width);

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
        createThreeArmShrimp(shrimp_component, curr_direction_facing, position, width, height);
        break;
    }    

    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(shrimp);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildShrimpTree();
    
    return shrimp;
}

Entity createSawfish(vec2 position, float width, float height, COLOUR_STATE color_state)
{
    Entity sawfish = Entity();

	addGeneralCreatureComponents(sawfish, position, width, height, color_state, CREATURETYPE::SAWFISH, TEXTURE_ASSET_ID::SAWFISH_01);

    registry.pathfinders.emplace(sawfish);
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
