#pragma once
#include "builtin_types.h"
#include <array>
#include "component.h"
#include <SDL_events.h>

struct Physical {
    builtin::Vec2 position{0, 0};
    builtin::Vec2 velocity{0, 0};
    builtin::Vec2 acceleration{0, 0};
    builtin::Vec2 size{100, 100};
    bool welded{true};//maybe put these in a bitset
    bool falling{true};
    bool obstructed{true};
    float elasticity{.3f};
    float friction{.3f};
    float mass{1.f};
};
void physics_system(component::StorageView<Physical> components, double delta_s);
namespace solvers {
    float effective_elasticity(float e1, float e2);
    std::array<builtin::Vec2, 2> velocities_after_collision(float e1, float m1, const builtin::Vec2& u1, float e2, float m2, const builtin::Vec2& u2);
    bool is_overlapping(const builtin::Rect &a, const builtin::Rect &b);
    bool is_point_in_rect(const builtin::Vec2& point, const builtin::Rect& rect);
    std::array<builtin::Vec2, 4> corner_points(const builtin::Vec2& pos, const builtin::Vec2& size);
    builtin::Rect to_rect(const builtin::Vec2& pos, const builtin::Vec2& size);
}
