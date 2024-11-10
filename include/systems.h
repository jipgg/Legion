#pragma once
#include "builtin.h"
#include <array>
#include "component.h"
#include <SDL_events.h>

struct physical_component {
    builtin::vector2 position{0, 0};
    builtin::vector2 velocity{0, 0};
    builtin::vector2 acceleration{0, 0};
    builtin::vector2 size{100, 100};
    bool welded{true};//maybe put these in a bitset
    bool falling{true};
    bool obstructed{true};
    float elasticity{.3f};
    float friction{.3f};
    float mass{1.f};
};
void physics_system(component::storage_view<physical_component> components, double delta_s);
namespace solve {
    float effective_elasticity(float e1, float e2);
    std::array<builtin::vector2, 2> velocities_after_collision(float e1, float m1, const builtin::vector2& u1, float e2, float m2, const builtin::vector2& u2);
    bool is_overlapping(const builtin::rectangle &a, const builtin::rectangle &b);
    bool is_point_in_rect(const builtin::vector2& point, const builtin::rectangle& rect);
    std::array<builtin::vector2, 4> corner_points(const builtin::vector2& pos, const builtin::vector2& size);
    builtin::rectangle to_rect(const builtin::vector2& pos, const builtin::vector2& size);
}
