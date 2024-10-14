#include "Legion.h"
namespace slv = solvers;
#include <SDL.h>
#include "ECS.h"

//batch systems
void physics_system(std::span<Physical> components, double delta_s) {
    constexpr float big_mass{1e25};
    constexpr float gravity = .3f;
    constexpr int x = 0;
    constexpr int y = 1;
    constexpr float deadzone = .1f;
    for (Physical& a : components) {
        if (a.welded) continue;
        if (a.falling) {
            a.acceleration = {0, -gravity};
        } else if (blaze::abs(a.velocity.at(x)) > deadzone) {
            const auto dir = a.velocity.at(x) > 0.f ? -1.f : 1.f;
            a.acceleration.at(x) = dir * a.friction_coeff * gravity;
        } else {
            a.acceleration.at(x) = 0;
            a.velocity.at(x) = 0;
        }
        a.velocity += a.acceleration * delta_s;
        a.position += a.velocity * delta_s;
        const auto [a_left, a_right, a_top, a_bottom] = a.corner_points();
        for (Physical& b : components) {
            if (&a == &b) continue;
            const float b_mass = b.welded ? big_mass : b.mass;
            auto [a_vel, b_vel] = slv::velocities_after_collision(
                a.elasticity_coeff,
                a.mass, a.velocity,
                b.elasticity_coeff,
                b_mass, b.velocity);
            const Recti64 b_bounds = b.bounds();
            if (slv::is_in_bounds(a_left, b_bounds)) {
                if (a.velocity.at(x) < 0) {
                    a.obstructed = true;
                    a.position.at(x) = b.position.at(x) + b.size.width();
                    a.velocity.at(x) = a_vel.at(x);
                    if (not b.welded) {
                        b.velocity.at(x) = b_vel.at(x);
                    }
                } else a.obstructed = false;
            } else if (slv::is_in_bounds(a_right, b_bounds)) {
                if (a.velocity.at(x) > 0) {
                    a.obstructed = true;
                    a.position.at(x) = b.position.at(x) - a.size.width();
                    a.velocity.at(x) = a_vel.at(x);
                    if (not b.welded) {
                        b.velocity.at(x) = b_vel.at(x);
                    }
                } else a.obstructed = false;
            }
            if (slv::is_in_bounds(a_top, b_bounds)) {
                a.falling = false;
                if (a.velocity.at(y) < 0) {
                    a.position.at(y) = b.position.at(y) + b.size.height();
                    a.velocity.at(y) = a_vel.at(y);
                    if (not b.welded) {
                        b.velocity.at(y) = b_vel.at(y);
                    }
                } else a.falling = true;
            }
            if (slv::is_in_bounds(a_bottom, b_bounds)) {
                if (a.velocity.at(y) > 0) {
                    a.position.at(y) = b.position.at(y) - a.size.height();
                    a.velocity.at(y) = a_vel.at(y);
                    if (not b.welded) {
                        b.velocity.at(y) = b_vel.at(y);
                    }
                }
            } 
        }
    }
}
void render_system(std::span<Renderable> components) {
    for (auto& e : components) e.render_fn();
}
void update_system(std::span<Updatable> components, double delta_s) {
    for (auto& e : components) e.update_fn(delta_s);
}
//single systems
void player_input_system(const Playable& plr, Physical& phys, double delta_s) {
    constexpr float jump_power = 10;
    const uint8_t* key_states = SDL_GetKeyboardState(nullptr);
    int direction{0};
    if (key_states[plr.right]) ++direction;
    if (key_states[plr.left]) --direction;
    if (phys.falling) return;

    if (key_states[plr.jump]) {
        phys.velocity.at(2) += plr.jump_power;
    };
    if (direction) {
        phys.velocity.at(0) = direction * plr.walk_speed * delta_s;
    }
}
