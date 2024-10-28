#pragma once
#include "common/common.h"
#include <array>
#include "component.h"
#include "types.h"
#include <SDL_events.h>
namespace systems {
void physics(component::storage_view<types::physical_component> components, double delta_s);
namespace solvers {
    float effective_elasticity(float e1, float e2);
    std::array<common::vec2f, 2> velocities_after_collision(float  e1, float m1, const common::vec2f& u1, float e2, float m2, const common::vec2f& u2);
    bool is_overlapping(const common::recti64 &a, const common::recti64 &b);
    bool is_in_bounds(const common::vec2i16& point, const common::recti64& bounds);
}
}
