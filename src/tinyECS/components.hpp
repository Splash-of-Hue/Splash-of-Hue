#pragma once
#include <string>
#include <map>
#include "trace.h"

// Component flags
enum ComponentType
{
    Invalid = 0,
    Detected = 1 << 0,
    Exited = 1 << 1,
    Invader = 1 << 2,
    Moving = 1 << 3,
    Position = 1 << 4,
    Queued = 1 << 5,
    ReleaseTimer = 1 << 6,
    Tower = 1 << 7
};

inline ComponentType operator|(ComponentType a, ComponentType b)
{
    return static_cast<ComponentType>(static_cast<int>(a) | static_cast<int>(b));
}
inline ComponentType operator&(ComponentType a, ComponentType b)
{
    return static_cast<ComponentType>(static_cast<int>(a) & static_cast<int>(b));
}

struct DetectedComponent {};

struct ExitedComponent {};

struct InvaderComponent {};

struct MovingComponent {};

struct PositionComponent
{
    float x;
    float y;
};

struct QueuedComponent {};

struct ReleaseTimerComponent
{
    float timer;
};

struct TowerComponent
{
    float range;
};
