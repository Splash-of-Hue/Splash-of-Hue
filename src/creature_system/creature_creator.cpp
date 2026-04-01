#include "creature_system/creature_creator.hpp"
#include "ecs/registry.hpp"
#include "shapes/shape_build_init.hpp"
#include "creature_system/creature_behaviour_tree_builder.hpp"
#include "creature_system/creature_components.hpp"
#include "emissions/emission_constants.hpp"
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
    PolygonMesh& anglerfish_hitbox = registry.hitboxes.emplace(anglerfish);
    float hitbox_height = height * 0.32;
    float hitbox_width = width * 0.35;
    anglerfish_hitbox.vertices.push_back(vec2(position.x - hitbox_width, position.y - hitbox_height));
    anglerfish_hitbox.vertices.push_back(vec2(position.x + hitbox_width, position.y - hitbox_height));
    anglerfish_hitbox.vertices.push_back(vec2(position.x + hitbox_width, position.y + hitbox_height));
    anglerfish_hitbox.vertices.push_back(vec2(position.x - hitbox_width, position.y + hitbox_height));
    anglerfish_hitbox.centre = position;
    anglerfish_hitbox.base_vertices = anglerfish_hitbox.vertices;

    registry.unloadables.emplace(anglerfish, Unloadable{position});

    Pathfinder& pathfinder = registry.pathfinders.emplace(anglerfish);
    pathfinder.target = registry.players.entities[0];
    pathfinder.type = PATHFINDING_TYPE::DIRECT;
    pathfinder.max_speed = ANGLERFISH_FOLLOWING_SPEED;
    CreatureBehaviour& creatureBehaviour = registry.creatureBehaviours.emplace(anglerfish);
    CreatureBehaviourTreeBuilder builder;
    creatureBehaviour.behaviour_tree = builder.buildAnglerfishTree();
    
    //create emission
    Entity& emission_entity = createCircle(position, 1.f, RENDER_LAYER::CREATURES, DEFAULT_COLOUR_VEC4);
    registry.scales.emplace(emission_entity);
    registry.unloadables.emplace(emission_entity, Unloadable{ position });
    LightEmission& light_emission_component = registry.lightEmissions.emplace(emission_entity);
    light_emission_component.MAX_RADIUS = EMISSION_SIZE::ANGLERFISH_EMISSION;
    light_emission_component.TIME_TO_GROW = TIME_TO_GROW;
    light_emission_component.offset = EMISSION_OFFSET.at("ANGLERFISH");
    light_emission_component.lightSourceEntity = anglerfish;

    PolygonMesh& emission_hitbox = registry.hitboxes.emplace(emission_entity);
    emission_hitbox = createCircleHitbox(position, EMISSION_SIZE::ANGLERFISH_EMISSION * 0.5, 16);

    LightSource& light_source_component = registry.lightSources.emplace(anglerfish);
    light_source_component.lightEmissionEntity = emission_entity;

    LevelEntity& level_entity_emission_component = registry.levelEntities.emplace(emission_entity);
    level_entity_emission_component.level_uid = level_uid;

    return anglerfish;
}

Entity CreatureCreator::createCrab(const int level_uid, vec2 position, float width, float height, COLOUR_STATE color_state, DIRECTION curr_direction_facing)
{
    Entity crab = Entity();

    // TODO add dedicated crab asset and texture ID
    // TODO: ocWt9dv
	addGeneralCreatureComponents(level_uid, crab, position, width, height, color_state, CREATURETYPE::CRAB, TEXTURE_ASSET_ID::CRAB_01);
    Angle& direction_facing = registry.angles.get(crab);
    vec2& direction = registry.directions.emplace(crab);
	// override what's in addGeneralCreatureComponents since crab is resizable
    registry.scales.get(crab) = vec2(width, height);
    direction = vec2(0);
    if (curr_direction_facing == DIRECTION::UP) {
        direction_facing.degrees = DEGREES_DOWN * 3;
        direction.y = -1.f;
        registry.visualRotationOffsets.insert(crab, { -(DEGREES_DOWN * 3) });
    }
    else if (curr_direction_facing == DIRECTION::DOWN) {
        direction_facing.degrees = DEGREES_DOWN;
        direction.y = 1.f;
        registry.visualRotationOffsets.insert(crab, { -DEGREES_DOWN });
    }
    else if (curr_direction_facing == DIRECTION::RIGHT) {
        direction_facing.degrees = 0.f;
        direction.x = 1.f;
        // no offset needed
    }
    else {
        direction_facing.degrees = DEGREES_DOWN * 2;
        direction.x = -1.f;
        registry.visualRotationOffsets.insert(crab, { -(DEGREES_DOWN * 2) });
    }
    registry.unloadables.emplace(crab, Unloadable{ position });
    const Animatable animatable = {
       ANIMATABLE_TYPE::CRAB,
       ANIMATION_BEHAVIOR::LOOP,
       ANIMATION_TRIGGER::IDLE,
       8
    };
    registry.animatables.insert(crab, animatable);

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

    Collidable& c = registry.collidables.get(minnows);
    c.inelastic = false;
    c.overlay = false;
    PolygonMesh& minnows_hitbox = registry.hitboxes.emplace(minnows);
    float hitbox_height = NUM_MINNOWS * height * 0.5;
    float hitbox_width = NUM_MINNOWS * width * 0.5;
    minnows_hitbox.vertices.push_back(vec2(position.x - hitbox_width, position.y - hitbox_height));
    minnows_hitbox.vertices.push_back(vec2(position.x + hitbox_width, position.y - hitbox_height));
    minnows_hitbox.vertices.push_back(vec2(position.x + hitbox_width, position.y + hitbox_height));
    minnows_hitbox.vertices.push_back(vec2(position.x - hitbox_width, position.y + hitbox_height));
    minnows_hitbox.centre = position;
    minnows_hitbox.base_vertices = minnows_hitbox.vertices;

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
    registry.sawfishes.emplace(sawfish);

    PolygonMesh& sawfish_hitbox = registry.hitboxes.emplace(sawfish);
    float hitbox_height = height * 0.15;
    float hitbox_width = width * 0.15;
    sawfish_hitbox.vertices.push_back(vec2(position.x - hitbox_width, position.y - hitbox_height));
    sawfish_hitbox.vertices.push_back(vec2(position.x + hitbox_width, position.y - hitbox_height));
    sawfish_hitbox.vertices.push_back(vec2(position.x + hitbox_width, position.y + hitbox_height));
    sawfish_hitbox.vertices.push_back(vec2(position.x - hitbox_width, position.y + hitbox_height));
    sawfish_hitbox.centre = position;
    sawfish_hitbox.base_vertices = sawfish_hitbox.vertices;

    PolygonMesh& sawfish_hurtbox = registry.hurtboxes.emplace(sawfish);
    sawfish_hurtbox.vertices.push_back(vec2(position.x + hitbox_width, position.y - hitbox_height));
    sawfish_hurtbox.vertices.push_back(vec2(position.x + hitbox_width, position.y + hitbox_height));
    sawfish_hurtbox.vertices.push_back(vec2(position.x + (hitbox_width*2), position.y)); //tip of sawfish nose
    sawfish_hurtbox.centre = position;
    sawfish_hurtbox.base_vertices = sawfish_hurtbox.vertices;

    registry.unloadables.emplace(sawfish, Unloadable{ position });

    Pathfinder& pathfinder = registry.pathfinders.emplace(sawfish);
    pathfinder.target = registry.players.entities[0];
    pathfinder.type = PATHFINDING_TYPE::DIRECT;
    pathfinder.max_speed = SAWFISH_FOLLOWING_SPEED;
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
