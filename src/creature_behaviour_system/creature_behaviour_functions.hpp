#pragma once

#include "common.hpp"
#include "ecs/components.hpp"
#include "ecs/registry.hpp"
#include "ecs/tiny_ecs.hpp"

// System responsible for handling creature behaviours. 
void updateAnglerfish(float elapsed_ms, Entity& anglerfish_entity);
void updateCrab(Entity& crab_entity);
void updateCreature(Entity& creature_entity);
void updateMinnows(float elapsed_ms, Entity& minnow_entity);
void updateSawfish(float elapsed_ms, Entity& sawfish_entity);
void updateShrimp(Entity& shrimp_entity);
void updateWhale(Entity& whale_entity);

// Coloured behaviours for each creature.
void neutralAnglerfish(Entity& anglerfish_entity);
void blueAnglerfish(float elapsed_ms, Entity& anglerfish_entity);
void redAnglerfish(float elapsed_ms, Entity& anglerfish_entity);

void neutralCrab(Entity& crab_entity);
void blueCrab(Entity& crab_entity);
void redCrab(Entity& crab_entity);

void neutralCreature(Entity& creature_entity);
void blueCreature(Entity& creature_entity);
void redCreature(Entity& creature_entity);

void neutralMinnows(Entity& minnow_entity);
void blueMinnows(Entity& minnow_entity);
void redMinnows(Entity& minnow_entity);

void neutralSawfish(float elapsed_ms, Entity& sawfish_entity);
void blueSawfish(Entity& sawfish_entity);
void redSawfish(float elapsed_ms, Entity& sawfish_entity);

void neutralShrimp(Entity& shrimp_entity);
void blueShrimp(Entity& shrimp_entity);
void redShrimp(Entity& shrimp_entity);

void neutralWhale(Entity& whale_entity);
void blueWhale(Entity& whale_entity);
void redWhale(Entity& whale_entity);

// Pathfinder
void follow_player(float elapsed_ms, Entity& follower);