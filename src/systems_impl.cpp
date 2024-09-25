#include "Legion.h"

void PhysicsSystem::operator()() {
    for (MySet& set : cache()) {
        Position& p = std::get<0>(set);
        Velocity& v = std::get<1>(set);
        Acceleration& a = std::get<2>(set);
        PhysicalProperties& prop = std::get<3>(set);
        CollisionRect& rect = std::get<4>(set);
    }
}
