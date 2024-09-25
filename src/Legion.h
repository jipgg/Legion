#pragma once
#include "common.h"
#include "ECS.h"
//components
struct Position: ECS::Component {
    V2 v{0, 0};
};
struct Velocity: ECS::Component  {
    V2 v{0, 0};
};
struct Acceleration: ECS::Component {
    V2 v{0, 0};
};
struct CollisionRect: ECS::Component {
    V2 offset;
    V2 size;
};
struct Transform: ECS::Component {
    M3x3 value{};
};
struct PhysicalProperties: ECS::Component {
    float mass{10.f};
    float elasticity{.3f};
    float friction{.4f};
};
//systems
struct PhysicsSystem: ECS::System<Position, Velocity, Acceleration, PhysicalProperties, CollisionRect> {
    float gravity{.6f};
    void operator()() override;
};
