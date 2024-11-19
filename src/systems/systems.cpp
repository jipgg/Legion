#include "systems.h"
#include "builtin_types.h"
#include <SDL.h>
using component::StorageView;
void physics_system(StorageView<Physical> components, double delta_s) {
    constexpr double big_mass{1e25};
    constexpr double gravity = -120.f;
    constexpr int x = 0;
    constexpr int y = 1;
    constexpr float deadzone = .1f;
    for (Physical& a : components) {
        if (a.welded) continue;
        if (a.falling) {
            a.acceleration = {0, -gravity};
        } else if (blaze::abs(a.velocity.at(x)) > deadzone) {
            const auto dir = a.velocity.at(x) > 0.f ? -1.f : 1.f;
            a.acceleration.at(x) = dir * a.friction * gravity;
        } else {
            a.acceleration.at(x) = 0;
            a.velocity.at(x) = 0;
        }
        a.velocity += a.acceleration * delta_s;
        a.position += a.velocity * delta_s;
        const auto [a_left, a_right, a_top, a_bottom] = solvers::corner_points(a.position, a.size);
        for (Physical& b : components) {
            if (&a == &b) continue;
            const float b_mass = b.welded ? big_mass : b.mass;
            auto [a_vel, b_vel] = solvers::velocities_after_collision(
                a.elasticity,
                a.mass, a.velocity,
                b.elasticity,
                b_mass, b.velocity);
            const builtin::Rect b_bounds = solvers::to_rect(b.position, a.position);
            if (solvers::is_point_in_rect(a_left, b_bounds)) {
                if (a.velocity.at(x) < 0) {
                    a.obstructed = true;
                    a.position.at(x) = b.position.at(x) + b.size.at(x);
                    a.velocity.at(x) = a_vel.at(x);
                    if (not b.welded) {
                        b.velocity.at(x) = b_vel.at(x);
                    }
                } else a.obstructed = false;
            } else if (solvers::is_point_in_rect(a_right, b_bounds)) {
                if (a.velocity.at(x) > 0) {
                    a.obstructed = true;
                    a.position.at(x) = b.position.at(x) - a.size.at(x);
                    a.velocity.at(x) = a_vel.at(x);
                    if (not b.welded) {
                        b.velocity.at(x) = b_vel.at(x);
                    }
                } else a.obstructed = false;
            }
            if (solvers::is_point_in_rect(a_top, b_bounds)) {
                a.falling = false;
                if (a.velocity.at(y) < 0) {
                    a.position.at(y) = b.position.at(y) + b.size.at(y);
                    a.velocity.at(y) = a_vel.at(y);
                    if (not b.welded) {
                        b.velocity.at(y) = b_vel.at(y);
                    }
                } else a.falling = true;
            }
            if (solvers::is_point_in_rect(a_bottom, b_bounds)) {
                if (a.velocity.at(y) > 0) {
                    a.position.at(y) = b.position.at(y) - a.size.at(y);
                    a.velocity.at(y) = a_vel.at(y);
                    if (not b.welded) {
                        b.velocity.at(y) = b_vel.at(y);
                    }
                }
            } 
        }
    }
}
