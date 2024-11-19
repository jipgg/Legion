#include "systems.h"
#include "builtin_types.h"
using builtin::Vec2;
using builtin::Rect;

namespace solvers {
float effective_elasticity(float e1, float e2) {
    return (e1 + e2) / 2.f;
}
std::array<Vec2, 2> velocities_after_collision(
    float  e1, float m1, const Vec2& u1,
    float e2, float m2, const Vec2& u2) {
    const float e = effective_elasticity(e1, e2);
    return {
        Vec2{ (m1 * u1 + m2 * u2 - m2 * e * (u1 - u2)) / (m1 + m2) },
        Vec2{ (m1 * u1 + m2 * u2 + m1 * e * (u1 - u2)) / (m1 + m2) }
    };
}
bool is_overlapping(const Rect&a, const Rect&b) {
// If one rectangle is on left side of the other
	if (( a.x + a.w) < b.x or ( b.x + b.w ) < a.x) {
		return false;}
	// If one rectangle is under the other
	if (a.y > (b.y + b.h) or b.y > (a.y + a.h)) {
		return false;}
	return true;
}
bool is_point_in_rect(const Vec2& point, const Rect& rect) {
    return point[0] >= rect.x
        and point[0] <= rect.x + rect.w
        and point[1] >= rect.y
        and point[1] <= rect.y + rect.h;
}
std::array<Vec2, 4> corner_points(const Vec2& pos, const Vec2& size) {
        Vec2 left = pos + Vec2{0, size.at(1) / 2.f};
        Vec2 right = pos + Vec2{size.at(0), size.at(1) / 2.f};
        Vec2 top = pos + Vec2{size.at(0) / 2.f, 0};
        Vec2 bottom = pos + Vec2{size.at(0) / 2.0, size.at(1)};
        return {left, right, top, bottom};
}
Rect to_rect(const Vec2& pos, const Vec2& size) {
    return Rect{pos[0], pos[1], size[0], size[1]};
}
}
