#pragma once
#include <blaze/Blaze.h>
#include <cassert>
using V2 = blaze::StaticVector<float, 2>;
using M3x3 = blaze::StaticMatrix<float, 3, 2>;
struct Rect {V2 offset; float width, height;};
constexpr float get_x(V2 v) {
    return v[0];
}
constexpr float get_y(V2 v) {
    return v[1];
}

#define Expects(precondition) assert(precondition and "Expects")
#define Ensures(postcondition) assert(postcondition and "Ensures")
