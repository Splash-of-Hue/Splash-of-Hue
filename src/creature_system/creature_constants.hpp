#pragma once

#include <map>

enum CREATURETYPE
{
	ANGLERFISH = 0,
	CRAB = ANGLERFISH + 1,
	MINNOWS = CRAB + 1,
	SAWFISH = MINNOWS + 1,
	SHRIMP = SAWFISH + 1,
	WHALE = SHRIMP + 1,
	UNDEFINED = WHALE + 1
};

// Creature specific constants
struct Anglerfish
{
};

const float ANGLERFISH_FOLLOWING_SPEED = 1700.f;

struct Crab
{
};

const int NUM_MINNOWS = 25;
const float MINNOW_BLUE_ALIGNMENT_FACTOR = 0.002f;
const float MINNOW_BLUE_COHESION_FACTOR = 5.5f;
const float MINNOW_BLUE_SEPARATION_FACTOR = 10.5f;
const float MINNOW_BLUE_TARGET_FACTOR = 10.0f;

const float MINNOW_NEUTRAL_ALIGNMENT_FACTOR = 0.02f;
const float MINNOW_NEUTRAL_COHESION_FACTOR = 0.08f;
const float MINNOW_NEUTRAL_SEPARATION_FACTOR = 5.5f;
const float MINNOW_NEUTRAL_TARGET_FACTOR = 0.0f;

const float MINNOW_RED_ALIGNMENT_FACTOR = 0.08f;
const float MINNOW_RED_COHESION_FACTOR = 0.001f;
const float MINNOW_RED_SEPARATION_FACTOR = 10.5f;
const float MINNOW_RED_TARGET_FACTOR = -10.0f;


struct Sawfish
{
	
};

const float SAWFISH_CHARGING_SPEED = 500.f;
const float SAWFISH_WANDERING_SPEED = 100.f;
const float SAWFISH_FOLLOWING_SPEED = 1700.f;

enum SHRIMP_ARM_COLOUR
{
	NEUTRAL = 0,
	BLUE = NEUTRAL + 1,
	RED = BLUE + 1
};

const float SHRIMP_ARM_TRANSLATION_X = 80.0f; // How much to translate the shrimp arm away from the body on the x axis
const float SHRIMP_ARM_TRANSLATION_Y = 80.0f; // How much to translate the shrimp arm away from the body on the y axis

struct Whale
{
};

// for creatures that pathfind. TODO, edit based on experiments with how close each creature should be
constexpr float pathfinderMinDistToTarget(CREATURETYPE creatureType) {
	switch (creatureType)
	{
	case ANGLERFISH:
		return 2.5f;
	case SAWFISH:
		return 1.0f;
	default:
		return 0.0f;
	}
}

const float COOLDOWN_TIMER_DEFAULT = 3000.0f;
const float TIMELIMIT_TIMER_DEFAULT = 3000.0f;

const float DEGREES_DOWN = 90;
