#pragma once
#include "common.h"
#include <array>
#include <span>
#include "components.h"
namespace legion {
namespace systems {
void render(std::span<Renderable> components);
void update(std::span<Updatable> components, double delta_s);
void physics(std::span<Physical> components, double delta_s);
void player_input(const Playable& plr, Physical& phys, double delta_s);
}
namespace solvers {
float effective_elasticity(float e1, float e2);
std::array<Vec2f, 2> velocities_after_collision(float  e1, float m1, const Vec2f& u1, float e2, float m2, const Vec2f& u2);
bool is_overlapping(const Recti64 &a, const Recti64 &b);
bool is_in_bounds(const Vec2i16& point, const Recti64& bounds);
}
}
