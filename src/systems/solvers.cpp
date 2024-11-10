#include "systems.h"
using builtin::vector2;
using builtin::rectangle;

float solve::effective_elasticity(float e1, float e2) {
    return (e1 + e2) / 2.f;
}
std::array<vector2, 2> solve::velocities_after_collision(
    float  e1, float m1, const vector2& u1,
    float e2, float m2, const vector2& u2) {
    const float e = effective_elasticity(e1, e2);
    return {
        vector2{ (m1 * u1 + m2 * u2 - m2 * e * (u1 - u2)) / (m1 + m2) },
        vector2{ (m1 * u1 + m2 * u2 + m1 * e * (u1 - u2)) / (m1 + m2) }
    };
}
bool solve::is_overlapping(const rectangle&a, const rectangle&b) {
// If one rectangle is on left side of the other
	if (( a.x + a.w) < b.x or ( b.x + b.w ) < a.x) {
		return false;}
	// If one rectangle is under the other
	if (a.y > (b.y + b.h) or b.y > (a.y + a.h)) {
		return false;}
	return true;
}
bool solve::is_point_in_rect(const vector2& point, const rectangle& rect) {
    return point[0] >= rect.x
        and point[0] <= rect.x + rect.w
        and point[1] >= rect.y
        and point[1] <= rect.y + rect.h;
}
std::array<vector2, 4> solve::corner_points(const vector2& pos, const vector2& size) {
        using v2 = builtin::vector2;
        v2 left = pos + v2{0, size.at(1) / 2.f};
        v2 right = pos + v2{size.at(0), size.at(1) / 2.f};
        v2 top = pos + v2{size.at(0) / 2.f, 0};
        v2 bottom = pos + v2{size.at(0) / 2.0, size.at(1)};
        return {left, right, top, bottom};
}
rectangle solve::to_rect(const vector2& pos, const vector2& size) {
    return rectangle{pos[0], pos[1], size[0], size[1]};
}
