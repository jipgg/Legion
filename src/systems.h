#pragma once
#include "common.h"
#include <array>
#include "component.h"
#include "types.h"
#include <SDL_events.h>
namespace systems {
void render(component::View<types::Renderable> components);
void update(component::View<types::Updatable> components, double delta_s);
void physics(component::View<types::Physical> components, double delta_s);
void player_input(const types::Playable& plr, types::Physical& phys, double delta_s);
void process_mouse_up(component::View<types::Clickable> cmps, const SDL_MouseButtonEvent& e);
void process_mouse_down(component::View<types::Clickable> cmps, const SDL_MouseButtonEvent& e);
namespace solvers {
    float effective_elasticity(float e1, float e2);
    std::array<common::Vec2f, 2> velocities_after_collision(float  e1, float m1, const common::Vec2f& u1, float e2, float m2, const common::Vec2f& u2);
    bool is_overlapping(const common::Recti64 &a, const common::Recti64 &b);
    bool is_in_bounds(const common::Vec2i16& point, const common::Recti64& bounds);
}
}
