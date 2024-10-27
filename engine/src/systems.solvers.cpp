#include "systems.h"
namespace slv = systems::solvers;
using common::Vec2f;
using common::Recti64;

float slv::effective_elasticity(float e1, float e2) {
    return (e1 + e2) / 2.f;
}
std::array<Vec2f, 2> slv::velocities_after_collision(
    float  e1, float m1, const Vec2f& u1,
    float e2, float m2, const Vec2f& u2) {
    const float e = effective_elasticity(e1, e2);
    return {
        Vec2f{ (m1 * u1 + m2 * u2 - m2 * e * (u1 - u2)) / (m1 + m2) },
        Vec2f{ (m1 * u1 + m2 * u2 + m1 * e * (u1 - u2)) / (m1 + m2) }
    };
}
bool slv::is_overlapping(const Recti64 &a, const Recti64 &b) {
// If one rectangle is on left side of the other
	if (( a.x() + a.width() ) < b.x() or ( b.x() + b.width() ) < a.x()) {
		return false;}
	// If one rectangle is under the other
	if (a.y() > ( b.y() + b.height() ) or b.y() > ( a.y() + a.height()) ) {
		return false;}
	return true;
}
bool slv::is_in_bounds(const common::Vec2i16& point, const Recti64& bounds) {
    return point.at(0) >= bounds.x()
        and point.at(0) <= bounds.x() + bounds.width()
        and point.at(1) >= bounds.y() 
        and point.at(1) <= bounds.y() + bounds.height();
}
